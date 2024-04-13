#include "networking/event_handlers/HitSoldierNetworkEventHandler.hpp"

#include "spdlog/spdlog.h"

#include <chrono>

namespace Soldank
{
HitSoldierNetworkEventHandler::HitSoldierNetworkEventHandler(const std::shared_ptr<IWorld>& world)
    : world_(world)
{
}

NetworkEventHandlerResult HitSoldierNetworkEventHandler::HandleNetworkMessageImpl(
  // TODO: remove from client: sender_connection_id
  unsigned int /*sender_connection_id*/,
  std::uint8_t soldier_id,
  float damage)
{
    world_->HitSoldier(soldier_id, damage);
    return NetworkEventHandlerResult::Success;
}
} // namespace Soldank
