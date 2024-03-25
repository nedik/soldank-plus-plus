#include "networking/event_handlers/PlayerLeaveNetworkEventHandler.hpp"

#include "spdlog/spdlog.h"

#include <chrono>

namespace Soldat
{
PlayerLeaveNetworkEventHandler::PlayerLeaveNetworkEventHandler(const std::shared_ptr<IWorld>& world)
    : world_(world)
{
}

NetworkEventHandlerResult PlayerLeaveNetworkEventHandler::HandleNetworkMessageImpl(
  unsigned int /*sender_connection_id*/,
  unsigned int soldier_id)
{
    const auto& state = world_->GetState();
    for (auto it = state->soldiers.begin(); it != state->soldiers.end();) {
        if (it->id == soldier_id) {
            it = state->soldiers.erase(it);
        } else {
            it++;
        }
    }

    return NetworkEventHandlerResult::Success;
}
} // namespace Soldat
