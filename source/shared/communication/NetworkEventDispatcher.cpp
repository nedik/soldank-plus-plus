#include <utility>

#include "communication/NetworkEvent.hpp"
#include "communication/NetworkEventDispatcher.hpp"
#include "communication/NetworkMessage.hpp"
#include "communication/NetworkPackets.hpp"

#include "core/math/Glm.hpp"

#include "spdlog/spdlog.h"

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

    NetworkEventObserverResult observer_result = NetworkEventObserverResult::Failure;

    switch (network_event) {
        case NetworkEvent::AssignPlayerId: {
            auto parsed = network_message.Parse<NetworkEvent, unsigned int>();
            if (!parsed.has_value()) {
                return { NetworkEventDispatchResult::ParseError, parsed.error() };
            }
            unsigned int assigned_player_id = std::get<1>(*parsed);

            spdlog::info("[ProcessNetworkMessage] AssignPlayerId: {}", assigned_player_id);
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

            spdlog::info("[ProcessNetworkMessage] ChatMessage: {}", chat_message);
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

            spdlog::info("[ProcessNetworkMessage] SpawnSoldier: {}, ({}, {})",
                         soldier_id,
                         spawn_position.x,
                         spawn_position.y);
            observer_result = network_event_observer_->OnSpawnSoldier(
              connection_metadata, soldier_id, spawn_position);
            break;
        }
        case NetworkEvent::SoldierInput: {
            auto parsed = network_message.Parse<NetworkEvent, SoldierInputPacket>();
            if (!parsed.has_value()) {
                return { NetworkEventDispatchResult::ParseError, parsed.error() };
            }
            SoldierInputPacket update_soldier_state_packet = std::get<1>(*parsed);
            unsigned int input_sequence_id = update_soldier_state_packet.input_sequence_id;
            unsigned int game_tick = update_soldier_state_packet.game_tick;
            unsigned int soldier_id = update_soldier_state_packet.player_id;
            glm::vec2 soldier_position = { update_soldier_state_packet.position_x,
                                           update_soldier_state_packet.position_y };
            glm::vec2 mouse_position = { update_soldier_state_packet.mouse_position_x,
                                         update_soldier_state_packet.mouse_position_y };
            Control player_control = update_soldier_state_packet.control;

            spdlog::info("[ProcessNetworkMessage] SoldierInput({}): {}, ({}, {})",
                         game_tick,
                         soldier_id,
                         soldier_position.x,
                         soldier_position.y);
            observer_result = network_event_observer_->OnSoldierInput(connection_metadata,
                                                                      input_sequence_id,
                                                                      soldier_id,
                                                                      soldier_position,
                                                                      mouse_position,
                                                                      player_control);
            break;
        }
        case NetworkEvent::SoldierState: {
            auto parsed = network_message.Parse<NetworkEvent, SoldierStatePacket>();
            if (!parsed.has_value()) {
                return { NetworkEventDispatchResult::ParseError, parsed.error() };
            }
            SoldierStatePacket update_soldier_state_packet = std::get<1>(*parsed);
            unsigned int game_tick = update_soldier_state_packet.game_tick;
            unsigned int soldier_id = update_soldier_state_packet.player_id;
            glm::vec2 soldier_position = { update_soldier_state_packet.position_x,
                                           update_soldier_state_packet.position_y };
            glm::vec2 soldier_old_position = { update_soldier_state_packet.old_position_x,
                                               update_soldier_state_packet.old_position_y };

            AnimationType body_animation_type = update_soldier_state_packet.body_animation_type;
            unsigned int body_animation_frame = update_soldier_state_packet.body_animation_frame;
            int body_animation_speed = update_soldier_state_packet.body_animation_speed;
            AnimationType legs_animation_type = update_soldier_state_packet.legs_animation_type;
            unsigned int legs_animation_frame = update_soldier_state_packet.legs_animation_frame;
            int legs_animation_speed = update_soldier_state_packet.legs_animation_speed;

            glm::vec2 soldier_velocity = { update_soldier_state_packet.velocity_x,
                                           update_soldier_state_packet.velocity_y };
            glm::vec2 soldier_force = { update_soldier_state_packet.force_x,
                                        update_soldier_state_packet.force_y };

            bool on_ground = update_soldier_state_packet.on_ground;
            bool on_ground_for_law = update_soldier_state_packet.on_ground_for_law;
            bool on_ground_last_frame = update_soldier_state_packet.on_ground_last_frame;
            bool on_ground_permanent = update_soldier_state_packet.on_ground_permanent;

            std::uint8_t stance = update_soldier_state_packet.stance;

            float mouse_position_x = update_soldier_state_packet.mouse_position_x;
            float mouse_position_y = update_soldier_state_packet.mouse_position_y;

            bool using_jets = update_soldier_state_packet.using_jets;
            std::int32_t jets_count = update_soldier_state_packet.jets_count;

            unsigned int last_processed_input_id =
              update_soldier_state_packet.last_processed_input_id;

            spdlog::info("[ProcessNetworkMessage] SoldierState({}): {}, ({}, {})",
                         game_tick,
                         soldier_id,
                         soldier_position.x,
                         soldier_position.y);
            observer_result = network_event_observer_->OnSoldierState(connection_metadata,
                                                                      soldier_id,
                                                                      soldier_position,
                                                                      soldier_old_position,
                                                                      body_animation_type,
                                                                      body_animation_frame,
                                                                      body_animation_speed,
                                                                      legs_animation_type,
                                                                      legs_animation_frame,
                                                                      legs_animation_speed,
                                                                      soldier_velocity,
                                                                      soldier_force,
                                                                      on_ground,
                                                                      on_ground_for_law,
                                                                      on_ground_last_frame,
                                                                      on_ground_permanent,
                                                                      stance,
                                                                      mouse_position_x,
                                                                      mouse_position_y,
                                                                      using_jets,
                                                                      jets_count,
                                                                      last_processed_input_id);
            break;
        }
        case NetworkEvent::SoldierInfo: {
            auto parsed = network_message.Parse<NetworkEvent, SoldierInfoPacket>();
            if (!parsed.has_value()) {
                return { NetworkEventDispatchResult::ParseError, parsed.error() };
            }
            SoldierInfoPacket soldier_info_packet = std::get<1>(*parsed);
            unsigned int soldier_id = soldier_info_packet.soldier_id;

            spdlog::info("[ProcessNetworkMessage] SoldierInfo: {}", soldier_id);

            observer_result =
              network_event_observer_->OnSoldierInfo(connection_metadata, soldier_id);
            break;
        }
        case NetworkEvent::PlayerLeave: {
            auto parsed = network_message.Parse<NetworkEvent, unsigned int>();
            if (!parsed.has_value()) {
                return { NetworkEventDispatchResult::ParseError, parsed.error() };
            }
            unsigned int soldier_id = std::get<1>(*parsed);

            spdlog::info("[ProcessNetworkMessage] PlayerLeave: {}", soldier_id);

            observer_result =
              network_event_observer_->OnPlayerLeave(connection_metadata, soldier_id);
            break;
        }
        default: {
            // Since parsing directly writes bytes to variables, we need to handle a situation here
            // when we get out of range NetworkEvent For example, a user can send us a value of 80
            // when NetworkEvent has less than 80 values!
            spdlog::error("[ProcessNetworkMessage] ParseError, {}",
                          std::to_underlying(network_event));
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
