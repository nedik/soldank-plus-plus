#ifndef __NETWORK_EVENT_DISPATCHER_HPP__
#define __NETWORK_EVENT_DISPATCHER_HPP__

#include "communication/NetworkEvent.hpp"
#include "communication/NetworkMessage.hpp"

#include "core/math/Glm.hpp"
#include "core/state/Control.hpp"
#include "core/animations/Animation.hpp"
#include "core/types/BulletType.hpp"
#include "core/types/TeamType.hpp"
#include "core/types/WeaponType.hpp"

#include <memory>
#include <optional>
#include <variant>
#include <utility>
#include <functional>
#include <vector>

namespace Soldat
{
enum class NetworkEventDispatchResult
{
    Success = 0,
    ParseError,
    ObserverFailure,
    HandlerFailure
};

enum class NetworkEventObserverResult
{
    Success = 0,
    Failure = 1,
};

enum class NetworkEventHandlerResult
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
                                                      unsigned int active_weapon,
                                                      unsigned int last_processed_input_id) = 0;

    virtual NetworkEventObserverResult OnSoldierInfo(const ConnectionMetadata& connection_metadata,
                                                     unsigned int soldier_id) = 0;

    virtual NetworkEventObserverResult OnPlayerLeave(const ConnectionMetadata& connection_metadata,
                                                     unsigned int soldier_id) = 0;

    virtual NetworkEventObserverResult OnPingCheck(
      const ConnectionMetadata& connection_metadata) = 0;

    virtual NetworkEventObserverResult OnProjectileSpawn(
      const ConnectionMetadata& connection_metadata,
      unsigned int projectile_id,
      BulletType style,
      WeaponType weapon,
      float position_x,
      float position_y,
      float velocity_x,
      float velocity_y,
      std::int16_t timeout,
      float hit_multiply,
      TeamType team) = 0;
};

class INetworkEventHandler
{
public:
    virtual ~INetworkEventHandler() = default;
    virtual bool ShouldHandleNetworkEvent(NetworkEvent network_event) const = 0;
    virtual std::optional<ParseError> ValidateNetworkMessage(
      const NetworkMessage& network_message) const = 0;
    virtual NetworkEventHandlerResult HandleNetworkMessage(
      unsigned int sender_connection_id,
      const NetworkMessage& network_message) = 0;
};

template<typename... NetworkMessageArgs>
class NetworkEventHandlerBase : public INetworkEventHandler
{
public:
    bool ShouldHandleNetworkEvent(NetworkEvent network_event) const override
    {
        return network_event == GetTargetNetworkEvent();
    }

    std::optional<ParseError> ValidateNetworkMessage(
      const NetworkMessage& network_message) const override
    {
        auto parsed = GetNetworkMessageOrError(network_message);
        if (!parsed.has_value()) {
            return parsed.error();
        }

        return std::nullopt;
    }

    NetworkEventHandlerResult HandleNetworkMessage(unsigned int sender_connection_id,
                                                   const NetworkMessage& network_message) override
    {
        auto parsed = GetNetworkMessageOrError(network_message);
        if (!parsed.has_value()) {
            // Before using this method, the network_message should be validated. Here we just
            // assume the network_message is correct
            return NetworkEventHandlerResult::Failure;
        }

        return std::apply(
          [this, sender_connection_id](NetworkEvent /*ignore*/,
                                       NetworkMessageArgs... network_message_args) {
              return HandleNetworkMessageImpl(sender_connection_id, network_message_args...);
          },
          *parsed);
    }

protected:
    std::expected<std::tuple<NetworkEvent, NetworkMessageArgs...>, ParseError>
    GetNetworkMessageOrError(const NetworkMessage& network_message) const
    {
        return network_message.Parse<NetworkEvent, NetworkMessageArgs...>();
    }

    virtual NetworkEvent GetTargetNetworkEvent() const = 0;
    virtual NetworkEventHandlerResult HandleNetworkMessageImpl(unsigned int sender_connection_id,
                                                               NetworkMessageArgs...) = 0;
};

class NetworkEventDispatcher
{

public:
    using TDispatchResult =
      std::pair<NetworkEventDispatchResult,
                std::variant<ParseError, NetworkEventObserverResult, NetworkEventHandlerResult>>;

    NetworkEventDispatcher(
      std::shared_ptr<INetworkEventObserver> network_event_observer,
      const std::vector<std::shared_ptr<INetworkEventHandler>>& network_event_handlers);

    TDispatchResult ProcessNetworkMessage(const ConnectionMetadata& connection_metadata,
                                          const NetworkMessage& network_message);

private:
    static TDispatchResult HandleObserverResult(NetworkEventObserverResult observer_result);

    std::shared_ptr<INetworkEventObserver> network_event_observer_;
    std::vector<std::shared_ptr<INetworkEventHandler>> network_event_handlers_;
};
} // namespace Soldat

#endif
