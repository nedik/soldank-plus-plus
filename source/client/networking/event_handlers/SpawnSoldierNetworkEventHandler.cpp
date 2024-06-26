#include "networking/event_handlers/SpawnSoldierNetworkEventHandler.hpp"

#include "spdlog/spdlog.h"

namespace Soldank
{
SpawnSoldierNetworkEventHandler::SpawnSoldierNetworkEventHandler(
  const std::shared_ptr<IWorld>& world)
    : world_(world)
{
}

NetworkEventHandlerResult SpawnSoldierNetworkEventHandler::HandleNetworkMessageImpl(
  unsigned int /*sender_connection_id*/,
  std::uint8_t soldier_id,
  float spawn_position_x,
  float spawn_position_y)
{
    glm::vec2 spawn_position = { spawn_position_x, spawn_position_y };
    spdlog::info("OnSpawnSoldier: {}, ({}, {})", soldier_id, spawn_position.x, spawn_position.y);
    world_->SpawnSoldier(soldier_id, spawn_position);
    return NetworkEventHandlerResult::Success;
}
} // namespace Soldank
