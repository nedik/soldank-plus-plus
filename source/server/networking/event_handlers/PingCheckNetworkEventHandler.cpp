#include "networking/event_handlers/PingCheckNetworkEventHandler.hpp"

#include "spdlog/spdlog.h"

#include <chrono>

namespace Soldank
{

PingCheckNetworkEventHandler::PingCheckNetworkEventHandler(
  const std::shared_ptr<IGameServer>& game_server)
    : game_server_(game_server)
{
}

NetworkEventHandlerResult PingCheckNetworkEventHandler::HandleNetworkMessageImpl(
  unsigned int sender_connection_id)
{
    game_server_->SendNetworkMessage(sender_connection_id, { NetworkEvent::PingCheck });
    return NetworkEventHandlerResult::Success;
}
} // namespace Soldank
