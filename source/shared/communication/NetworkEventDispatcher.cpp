#include <utility>

#include "communication/NetworkEvent.hpp"
#include "communication/NetworkEventDispatcher.hpp"
#include "communication/NetworkPackets.hpp"

#include "core/math/Glm.hpp"

#include <iostream>

namespace Soldat
{
NetworkEventDispatcher::NetworkEventDispatcher(
  std::shared_ptr<INetworkEventObserver> network_event_observer)
    : network_event_observer_(std::move(network_event_observer))
{
}

NetworkEventDispatcher::TDispatchResult NetworkEventDispatcher::ProcessNetworkMessage(
  const ConnectionMetadata& connection_metadata,
  const NetworkMessage& network_message)
{
    auto network_event_or_error = network_message.GetNetworkEvent();
    if (!network_event_or_error.has_value()) {
        return { NetworkEventDispatchResult::ParseError, network_event_or_error.error() };
    }

    auto network_event = *network_event_or_error;
    std::cout << "[ProcessNetworkMessage] network_event = " << std::to_underlying(network_event)
              << std::endl;

    NetworkEventObserverResult observer_result = NetworkEventObserverResult::Failure;

    switch (network_event) {
        case NetworkEvent::AssignPlayerId: {
            auto parsed = network_message.Parse<NetworkEvent, unsigned int>();
            if (!parsed.has_value()) {
                return { NetworkEventDispatchResult::ParseError, parsed.error() };
            }
            unsigned int assigned_player_id = std::get<1>(*parsed);

            std::cout << "[ProcessNetworkMessage] AssignPlayerId: " << assigned_player_id
                      << std::endl;
            observer_result =
              network_event_observer_->OnAssignPlayerId(connection_metadata, assigned_player_id);
            break;
        }
        case NetworkEvent::ChatMessage: {
            auto parsed = network_message.Parse<NetworkEvent, std::string>();
            if (!parsed.has_value()) {
                return { NetworkEventDispatchResult::ParseError, parsed.error() };
            }
            std::string chat_message = std::get<1>(*parsed);

            std::cout << "[ProcessNetworkMessage] ChatMessage: " << chat_message << std::endl;
            observer_result =
              network_event_observer_->OnChatMessage(connection_metadata, chat_message);
            break;
        }
        case NetworkEvent::SpawnSoldier: {
            auto parsed = network_message.Parse<NetworkEvent, unsigned int, float, float>();
            if (!parsed.has_value()) {
                return { NetworkEventDispatchResult::ParseError, parsed.error() };
            }
            unsigned int soldier_id = std::get<1>(*parsed);
            glm::vec2 spawn_position{ std::get<2>(*parsed), std::get<3>(*parsed) };

            std::cout << "[ProcessNetworkMessage] SpawnSoldier: " << soldier_id << ", ("
                      << spawn_position.x << ", " << spawn_position.y << ")" << std::endl;
            observer_result = network_event_observer_->OnSpawnSoldier(
              connection_metadata, soldier_id, spawn_position);
            break;
        }
        case NetworkEvent::UpdateSoldierState: {
            auto parsed = network_message.Parse<NetworkEvent, UpdateSoldierStatePacket>();
            if (!parsed.has_value()) {
                return { NetworkEventDispatchResult::ParseError, parsed.error() };
            }
            UpdateSoldierStatePacket update_soldier_state_packet = std::get<1>(*parsed);
            unsigned int soldier_id = update_soldier_state_packet.id;
            glm::vec2 soldier_position = { update_soldier_state_packet.position_x,
                                           update_soldier_state_packet.position_y };

            std::cout << "[ProcessNetworkMessage] UpdateSoldierState: " << soldier_id << ", ("
                      << soldier_position.x << ", " << soldier_position.y << ")" << std::endl;
            observer_result = network_event_observer_->OnUpdateSoldierState(
              connection_metadata, soldier_id, soldier_position);
            break;
        }
        default: {
            // Since parsing directly writes bytes to variables, we need to handle a situation here
            // when we get out of range NetworkEvent For example, a user can send us a value of 80
            // when NetworkEvent has less than 80 values!
            std::cout << "[ProcessNetworkMessage] ParseError" << std::endl;
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
