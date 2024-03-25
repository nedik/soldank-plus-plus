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
  std::shared_ptr<INetworkEventObserver> network_event_observer,
  const std::vector<std::shared_ptr<INetworkEventHandler>>& network_event_handlers)
    : network_event_observer_(std::move(network_event_observer))
    , network_event_handlers_(network_event_handlers)
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

    for (const auto& network_event_handler : network_event_handlers_) {
        if (!network_event_handler->ShouldHandleNetworkEvent(network_event)) {
            continue;
        }

        auto parse_error_or_nothing =
          network_event_handler->ValidateNetworkMessage(network_message);
        if (parse_error_or_nothing.has_value()) {
            return { NetworkEventDispatchResult::ParseError, parse_error_or_nothing.value() };
        }

        auto handler_result = network_event_handler->HandleNetworkMessage(
          connection_metadata.connection_id, network_message);
        switch (handler_result) {
            case NetworkEventHandlerResult::Success:
                return { NetworkEventDispatchResult::Success, handler_result };
            case NetworkEventHandlerResult::Failure:
                return { NetworkEventDispatchResult::HandlerFailure, handler_result };
        }
    }

    // TODO: uncomment when handlers fully replaced observers
    // return { NetworkEventDispatchResult::ParseError, ParseError::InvalidNetworkEvent };

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

            unsigned int active_weapon = update_soldier_state_packet.active_weapon;

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
                                                                      active_weapon,
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
        case NetworkEvent::PingCheck: {
            spdlog::info("[ProcessNetworkMessage] PingCheck");
            observer_result = network_event_observer_->OnPingCheck(connection_metadata);
            break;
        }
        case NetworkEvent::ProjectileSpawn: {
            auto parsed = network_message.Parse<NetworkEvent, ProjectileSpawnPacket>();
            if (!parsed.has_value()) {
                return { NetworkEventDispatchResult::ParseError, parsed.error() };
            }
            ProjectileSpawnPacket projectile_spawn_packet = std::get<1>(*parsed);
            unsigned int projectile_id = projectile_spawn_packet.projectile_id;
            BulletType style = projectile_spawn_packet.style;
            WeaponType weapon = projectile_spawn_packet.weapon;
            float position_x = projectile_spawn_packet.position_x;
            float position_y = projectile_spawn_packet.position_y;
            float velocity_x = projectile_spawn_packet.velocity_x;
            float velocity_y = projectile_spawn_packet.velocity_y;
            std::int16_t timeout = projectile_spawn_packet.timeout;
            float hit_multiply = projectile_spawn_packet.hit_multiply;
            TeamType team = projectile_spawn_packet.team;

            spdlog::info("[ProcessNetworkMessage] ProjectileSpawn: {}", projectile_id);

            observer_result = network_event_observer_->OnProjectileSpawn(connection_metadata,
                                                                         projectile_id,
                                                                         style,
                                                                         weapon,
                                                                         position_x,
                                                                         position_y,
                                                                         velocity_x,
                                                                         velocity_y,
                                                                         timeout,
                                                                         hit_multiply,
                                                                         team);
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
