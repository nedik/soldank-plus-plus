#ifndef __NETWORK_EVENT_DISPATCHER_HPP__
#define __NETWORK_EVENT_DISPATCHER_HPP__

#include "communication/NetworkEvent.hpp"
#include "communication/NetworkMessage.hpp"

#include "core/math/Glm.hpp"
#include "core/state/Control.hpp"

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
                                                      unsigned int soldier_id,
                                                      glm::vec2 soldier_position,
                                                      const Control& player_control) = 0;

    virtual NetworkEventObserverResult OnSoldierState(const ConnectionMetadata& connection_metadata,
                                                      unsigned int soldier_id,
                                                      glm::vec2 soldier_position) = 0;
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
