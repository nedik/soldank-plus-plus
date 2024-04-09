#include "networking/event_handlers/PingCheckNetworkEventHandler.hpp"

#include "spdlog/spdlog.h"

#include <chrono>

namespace Soldank
{
PingCheckNetworkEventHandler::PingCheckNetworkEventHandler(
  const std::shared_ptr<ClientState>& client_state)
    : client_state_(client_state)
{
}

NetworkEventHandlerResult PingCheckNetworkEventHandler::HandleNetworkMessageImpl(
  unsigned int /*sender_connection_id*/)
{
    if (client_state_->ping_timer.IsRunning()) {
        client_state_->ping_timer.Stop();
    }

    return NetworkEventHandlerResult::Success;
}
} // namespace Soldank
