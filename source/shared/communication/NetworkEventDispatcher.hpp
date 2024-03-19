#ifndef __NETWORK_EVENT_DISPATCHER_HPP__
#define __NETWORK_EVENT_DISPATCHER_HPP__

#include "communication/NetworkEvent.hpp"
#include "communication/NetworkMessage.hpp"

#include "core/math/Glm.hpp"
#include "core/state/Control.hpp"
#include "core/animations/Animation.hpp"

#include <memory>
#include <variant>
#include <utility>
#include <functional>

namespace Soldat
{
enum class NetworkEventDispatchResult
{
    Success = 0,
    ParseError,
    ObserverFailure,
};

enum class NetworkEventObserverResult
{
    Success = 0,
    Failure = 1,
};

struct ConnectionMetadata
{
    unsigned int connection_id;
    std::function<void(const NetworkMessage&)> send_message_to_connection;
};

class INetworkEventObserver
{
public:
    virtual ~INetworkEventObserver() = default;

    virtual NetworkEventObserverResult OnAssignPlayerId(
      const ConnectionMetadata& connection_metadata,
      unsigned int assigned_player_id) = 0;
    virtual NetworkEventObserverResult OnChatMessage(const ConnectionMetadata& connection_metadata,
                                                     const std::string& chat_message) = 0;
    virtual NetworkEventObserverResult OnSpawnSoldier(const ConnectionMetadata& connection_metadata,
                                                      unsigned int soldier_id,
                                                      glm::vec2 spawn_position) = 0;
    virtual NetworkEventObserverResult OnSoldierInput(const ConnectionMetadata& connection_metadata,
                                                      unsigned int input_sequence_id,
                                                      unsigned int soldier_id,
                                                      glm::vec2 soldier_position,
                                                      glm::vec2 mouse_position,
                                                      const Control& player_control) = 0;

    virtual NetworkEventObserverResult OnSoldierState(const ConnectionMetadata& connection_metadata,
                                                      unsigned int soldier_id,
                                                      glm::vec2 soldier_position,
                                                      glm::vec2 soldier_old_position,
                                                      AnimationType body_animation_type,
                                                      unsigned int body_animation_frame,
                                                      int body_animation_speed,
                                                      AnimationType legs_animation_type,
                                                      unsigned int legs_animation_frame,
                                                      int legs_animation_speed,
                                                      glm::vec2 soldier_velocity,
                                                      glm::vec2 soldier_force,
                                                      bool on_ground,
                                                      bool on_ground_for_law,
                                                      bool on_ground_last_frame,
                                                      bool on_ground_permanent,
                                                      std::uint8_t stance,
                                                      float mouse_position_x,
                                                      float mouse_position_y,
                                                      bool using_jets,
                                                      std::int32_t jets_count,
                                                      unsigned int last_processed_input_id) = 0;

    virtual NetworkEventObserverResult OnSoldierInfo(const ConnectionMetadata& connection_metadata,
                                                     unsigned int soldier_id) = 0;

    virtual NetworkEventObserverResult OnPlayerLeave(const ConnectionMetadata& connection_metadata,
                                                     unsigned int soldier_id) = 0;

    virtual NetworkEventObserverResult OnPingCheck(
      const ConnectionMetadata& connection_metadata) = 0;
};

class NetworkEventDispatcher
{

public:
    using TDispatchResult =
      std::pair<NetworkEventDispatchResult, std::variant<ParseError, NetworkEventObserverResult>>;

    NetworkEventDispatcher(std::shared_ptr<INetworkEventObserver> network_event_observer);

    TDispatchResult ProcessNetworkMessage(const ConnectionMetadata& connection_metadata,
                                          const NetworkMessage& network_message);

private:
    static TDispatchResult HandleObserverResult(NetworkEventObserverResult observer_result);

    std::shared_ptr<INetworkEventObserver> network_event_observer_;
};
} // namespace Soldat

#endif
