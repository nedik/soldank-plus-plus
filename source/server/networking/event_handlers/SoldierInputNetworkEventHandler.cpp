#include "networking/event_handlers/SoldierInputNetworkEventHandler.hpp"

#include "spdlog/spdlog.h"

#include <chrono>

namespace Soldank
{
SoldierInputNetworkEventHandler::SoldierInputNetworkEventHandler(
  const std::shared_ptr<IWorld>& world,
  const std::shared_ptr<ServerState>& server_state)
    : world_(world)
    , server_state_(server_state)
{
}

NetworkEventHandlerResult SoldierInputNetworkEventHandler::HandleNetworkMessageImpl(
  unsigned int /*sender_connection_id*/,
  SoldierInputPacket soldier_input_packet)
{
    unsigned int input_sequence_id = soldier_input_packet.input_sequence_id;
    unsigned int soldier_id = soldier_input_packet.player_id;
    glm::vec2 soldier_position = { soldier_input_packet.position_x,
                                   soldier_input_packet.position_y };
    glm::vec2 mouse_position = { soldier_input_packet.mouse_position_x,
                                 soldier_input_packet.mouse_position_y };
    const Control& player_control = soldier_input_packet.control;

    // TODO: validate arguments
    // spdlog::info("{} Soldier pos from client: {} {}",
    //              input_sequence_id - 1,
    //              soldier_position.x,
    //              soldier_position.y);
    if (input_sequence_id <= server_state_->last_processed_input_id.at(soldier_id)) {
        spdlog::warn("*************** LATE PACKET ***************************");
        return NetworkEventHandlerResult::Failure;
    }

    server_state_->last_processed_input_id.at(soldier_id) = input_sequence_id;
    world_->UpdateRightButtonState(soldier_id, player_control.right);
    world_->UpdateLeftButtonState(soldier_id, player_control.left);
    world_->UpdateJumpButtonState(soldier_id, player_control.up);
    world_->UpdateCrouchButtonState(soldier_id, player_control.down);
    world_->UpdateProneButtonState(soldier_id, player_control.prone);
    world_->UpdateChangeButtonState(soldier_id, player_control.change);
    world_->UpdateThrowGrenadeButtonState(soldier_id, player_control.throw_grenade);
    world_->UpdateDropButtonState(soldier_id, player_control.drop);

    world_->UpdateMousePosition(soldier_id, { mouse_position.x, mouse_position.y });
    world_->UpdateFireButtonState(soldier_id, player_control.fire);
    world_->UpdateJetsButtonState(soldier_id, player_control.jets);

    return NetworkEventHandlerResult::Success;
}
} // namespace Soldank
