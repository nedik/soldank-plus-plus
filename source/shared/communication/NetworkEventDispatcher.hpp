#ifndef __NETWORK_EVENT_DISPATCHER_HPP__
#define __NETWORK_EVENT_DISPATCHER_HPP__

#include "communication/NetworkEvent.hpp"
#include "communication/NetworkMessage.hpp"

#include <memory>
#include <variant>
#include <utility>

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

class INetworkEventObserver
{
public:
    virtual ~INetworkEventObserver() = default;

    virtual NetworkEventObserverResult OnAssignPlayerId(unsigned int assigned_player_id) = 0;
    virtual NetworkEventObserverResult OnChatMessage(const std::string& chat_message) = 0;
};

class NetworkEventDispatcher
{

public:
    using TDispatchResult =
      std::pair<NetworkEventDispatchResult, std::variant<ParseError, NetworkEventObserverResult>>;

    NetworkEventDispatcher(std::shared_ptr<INetworkEventObserver> network_event_observer);

    TDispatchResult ProcessNetworkMessage(const NetworkMessage& network_message);

private:
    static TDispatchResult HandleObserverResult(NetworkEventObserverResult observer_result);

    std::shared_ptr<INetworkEventObserver> network_event_observer_;
};
} // namespace Soldat

#endif
