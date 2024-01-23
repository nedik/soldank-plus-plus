#include "networking/events/ServerNetworkEventObserver.hpp"
#include "communication/NetworkEventDispatcher.hpp"
#include "communication/NetworkMessage.hpp"

#include "spdlog/spdlog.h"

namespace Soldat
{
ServerNetworkEventObserver::ServerNetworkEventObserver(const std::shared_ptr<IWorld>& world)
    : world_(world)
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
  unsigned int soldier_id,
  glm::vec2 soldier_position,
  const Control& player_control)
{
    world_->UpdateRightButtonState(soldier_id, player_control.right);
    world_->UpdateLeftButtonState(soldier_id, player_control.left);
    world_->UpdateJumpButtonState(soldier_id, player_control.up);
    world_->UpdateCrouchButtonState(soldier_id, player_control.down);
    world_->UpdateProneButtonState(soldier_id, player_control.prone);
    world_->UpdateChangeButtonState(soldier_id, player_control.change);
    world_->UpdateThrowGrenadeButtonState(soldier_id, player_control.throw_grenade);
    world_->UpdateDropButtonState(soldier_id, player_control.drop);

    world_->UpdateMousePosition(soldier_id,
                                { player_control.mouse_aim_x, player_control.mouse_aim_y });
    world_->UpdateFireButtonState(soldier_id, player_control.fire);
    world_->UpdateJetsButtonState(soldier_id, player_control.jets);
    return NetworkEventObserverResult::Success;
}

NetworkEventObserverResult ServerNetworkEventObserver::OnSoldierState(
  const ConnectionMetadata& connection_metadata,
  unsigned int soldier_id,
  glm::vec2 soldier_position)
{
}
} // namespace Soldat
