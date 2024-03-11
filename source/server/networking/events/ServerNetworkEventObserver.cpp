#include "networking/events/ServerNetworkEventObserver.hpp"
#include "communication/NetworkEventDispatcher.hpp"
#include "communication/NetworkMessage.hpp"

#include "spdlog/spdlog.h"

namespace Soldat
{
ServerNetworkEventObserver::ServerNetworkEventObserver(
  const std::shared_ptr<IWorld>& world,
  const std::shared_ptr<ServerState>& server_state)
    : world_(world)
    , server_state_(server_state)
{
}

NetworkEventObserverResult ServerNetworkEventObserver::OnAssignPlayerId(
  const ConnectionMetadata& connection_metadata,
  unsigned int assigned_player_id)
{
    // TODO: this is not used
    return NetworkEventObserverResult::Success;
}

NetworkEventObserverResult ServerNetworkEventObserver::OnChatMessage(
  const ConnectionMetadata& connection_metadata,
  const std::string& chat_message)
{
    spdlog::info("OnChatMessage: {}", chat_message);
    return NetworkEventObserverResult::Success;
}

NetworkEventObserverResult ServerNetworkEventObserver::OnSpawnSoldier(
  const ConnectionMetadata& connection_metadata,
  unsigned int soldier_id,
  glm::vec2 spawn_position)
{
    return NetworkEventObserverResult::Success;
}

NetworkEventObserverResult ServerNetworkEventObserver::OnSoldierInput(
  const ConnectionMetadata& connection_metadata,
  unsigned int input_sequence_id,
  unsigned int soldier_id,
  glm::vec2 soldier_position,
  glm::vec2 mouse_position,
  const Control& player_control)
{
    // TODO: validate arguments
    // spdlog::info("{} Soldier pos from client: {} {}",
    //              input_sequence_id - 1,
    //              soldier_position.x,
    //              soldier_position.y);
    if (input_sequence_id <= server_state_->last_processed_input_id.at(soldier_id)) {
        spdlog::info("*************** LATE PACKET ***************************");
        return NetworkEventObserverResult::Success;
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
    return NetworkEventObserverResult::Success;
}

NetworkEventObserverResult ServerNetworkEventObserver::OnSoldierState(
  const ConnectionMetadata& connection_metadata,
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
  unsigned int last_processed_input_id)
{
    return NetworkEventObserverResult::Success;
}
} // namespace Soldat
