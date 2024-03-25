#include "networking/event_handlers/PingCheckNetworkEventHandler.hpp"

#include "spdlog/spdlog.h"

#include <chrono>

namespace Soldat
{
PingCheckNetworkEventHandler::PingCheckNetworkEventHandler(
  const std::shared_ptr<ClientState>& client_state)
    : client_state_(client_state)
{
}

NetworkEventHandlerResult PingCheckNetworkEventHandler::HandleNetworkMessageImpl(
  unsigned int /*sender_connection_id*/)
{
    if (client_state_->last_ping_check_time.has_value()) {
        auto current_time = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = (current_time - *client_state_->last_ping_check_time);

        client_state_->last_ping = (std::uint16_t)(diff.count() * 1000.0);
        client_state_->last_ping_check_time = std::nullopt;
        spdlog::info("Ping: {}", client_state_->last_ping);
    }

    return NetworkEventHandlerResult::Success;
}
} // namespace Soldat
