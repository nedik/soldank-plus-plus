#include <utility>

#include "communication/NetworkEvent.hpp"
#include "communication/NetworkEventDispatcher.hpp"

namespace Soldat
{
NetworkEventDispatcher::NetworkEventDispatcher(
  std::shared_ptr<INetworkEventObserver> network_event_observer)
    : network_event_observer_(std::move(network_event_observer))
{
}

NetworkEventDispatcher::TDispatchResult NetworkEventDispatcher::ProcessNetworkMessage(
  const NetworkMessage& network_message)
{
    auto network_event_or_error = network_message.GetNetworkEvent();
    if (!network_event_or_error.has_value()) {
        return { NetworkEventDispatchResult::ParseError, network_event_or_error.error() };
    }

    auto network_event = *network_event_or_error;

    NetworkEventObserverResult observer_result = NetworkEventObserverResult::Failure;

    switch (network_event) {
        case NetworkEvent::AssignPlayerId: {
            auto parsed = network_message.Parse<NetworkEvent, unsigned int>();
            if (!parsed.has_value()) {
                return { NetworkEventDispatchResult::ParseError, parsed.error() };
            }
            unsigned int assigned_player_id = std::get<1>(*parsed);
            observer_result = network_event_observer_->OnAssignPlayerId(assigned_player_id);
            break;
        }
        case NetworkEvent::ChatMessage: {
            auto parsed = network_message.Parse<NetworkEvent, std::string>();
            if (!parsed.has_value()) {
                return { NetworkEventDispatchResult::ParseError, parsed.error() };
            }
            std::string chat_message = std::get<1>(*parsed);
            observer_result = network_event_observer_->OnChatMessage(chat_message);
            break;
        }
        default: {
            // Since parsing directly writes bytes to variables, we need to handle a situation here
            // when we get out of range NetworkEvent For example, a user can send us a value of 80
            // when NetworkEvent has less than 80 values!
            return { NetworkEventDispatchResult::ParseError, ParseError::InvalidNetworkEvent };
        }
    }

    return HandleObserverResult(observer_result);
}

NetworkEventDispatcher::TDispatchResult NetworkEventDispatcher::HandleObserverResult(
  NetworkEventObserverResult observer_result)
{
    switch (observer_result) {
        case NetworkEventObserverResult::Success:
            return { NetworkEventDispatchResult::Success, observer_result };
        case NetworkEventObserverResult::Failure:
            return { NetworkEventDispatchResult::ObserverFailure, observer_result };
    }
}

} // namespace Soldat
