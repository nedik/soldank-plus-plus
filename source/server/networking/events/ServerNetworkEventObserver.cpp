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
    return NetworkEventObserverResult::Failure;
}

NetworkEventObserverResult ServerNetworkEventObserver::OnChatMessage(
  const ConnectionMetadata& connection_metadata,
  const std::string& chat_message)
{
    return NetworkEventObserverResult::Failure;
}

NetworkEventObserverResult ServerNetworkEventObserver::OnSpawnSoldier(
  const ConnectionMetadata& connection_metadata,
  unsigned int soldier_id,
  glm::vec2 spawn_position)
{
    return NetworkEventObserverResult::Failure;
}

NetworkEventObserverResult ServerNetworkEventObserver::OnSoldierInput(
  const ConnectionMetadata& connection_metadata,
  unsigned int input_sequence_id,
  unsigned int soldier_id,
  glm::vec2 soldier_position,
  glm::vec2 mouse_position,
  const Control& player_control)
{
    return NetworkEventObserverResult::Failure;
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
  std::uint8_t stance,
  float mouse_position_x,
  float mouse_position_y,
  bool using_jets,
  std::int32_t jets_count,
  unsigned int active_weapon,
  unsigned int last_processed_input_id)
{
    return NetworkEventObserverResult::Failure;
}

NetworkEventObserverResult ServerNetworkEventObserver::OnSoldierInfo(
  const ConnectionMetadata& connection_metadata,
  unsigned int soldier_id)
{
    return NetworkEventObserverResult::Failure;
}

NetworkEventObserverResult ServerNetworkEventObserver::OnPlayerLeave(
  const ConnectionMetadata& connection_metadata,
  unsigned int soldier_id)
{
    return NetworkEventObserverResult::Failure;
}

NetworkEventObserverResult ServerNetworkEventObserver::OnPingCheck(
  const ConnectionMetadata& connection_metadata)
{
    return NetworkEventObserverResult::Failure;
}

NetworkEventObserverResult ServerNetworkEventObserver::OnProjectileSpawn(
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
  TeamType team)
{
    return NetworkEventObserverResult::Failure;
}
} // namespace Soldat
