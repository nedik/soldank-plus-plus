#include "networking/event_handlers/KillSoldierNetworkEventHandler.hpp"

#include "spdlog/spdlog.h"

#include <chrono>

namespace Soldank
{
KillSoldierNetworkEventHandler::KillSoldierNetworkEventHandler(const std::shared_ptr<IWorld>& world)
    : world_(world)
{
}

NetworkEventHandlerResult KillSoldierNetworkEventHandler::HandleNetworkMessageImpl(
  // TODO: remove from client: sender_connection_id
  unsigned int /*sender_connection_id*/,
  unsigned int soldier_id)
{
    world_->KillSoldier(soldier_id);
    return NetworkEventHandlerResult::Success;
}
} // namespace Soldank
