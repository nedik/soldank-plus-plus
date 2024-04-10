#include "networking/event_handlers/KillCommandNetworkEventHandler.hpp"

#include "communication/NetworkEvent.hpp"
#include "spdlog/spdlog.h"

#include <chrono>

namespace Soldank
{

KillCommandNetworkEventHandler::KillCommandNetworkEventHandler(
  const std::shared_ptr<IWorld>& world,
  const std::shared_ptr<IGameServer>& game_server)
    : world_(world)
    , game_server_(game_server)
{
}

NetworkEventHandlerResult KillCommandNetworkEventHandler::HandleNetworkMessageImpl(
  unsigned int sender_connection_id)
{
    unsigned int soldier_id = game_server_->GetSoldierIdFromConnectionId(sender_connection_id);
    game_server_->SendNetworkMessageToAll({ NetworkEvent::KillSoldier, soldier_id });
    world_->KillSoldier(soldier_id);
    return NetworkEventHandlerResult::Success;
}
} // namespace Soldank
