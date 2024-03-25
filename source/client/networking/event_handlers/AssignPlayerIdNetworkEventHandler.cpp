#include "networking/event_handlers/AssignPlayerIdNetworkEventHandler.hpp"

#include "spdlog/spdlog.h"

namespace Soldat
{
AssignPlayerIdNetworkEventHandler::AssignPlayerIdNetworkEventHandler(
  const std::shared_ptr<IWorld>& world,
  const std::shared_ptr<ClientState>& client_state)
    : world_(world)
    , client_state_(client_state)
{
}

NetworkEventHandlerResult AssignPlayerIdNetworkEventHandler::HandleNetworkMessageImpl(
  unsigned int assigned_player_id)
{
    const auto& soldier = world_->CreateSoldier(assigned_player_id);
    client_state_->client_soldier_id = soldier.id;
    spdlog::info("OnAssignPlayerId: {} {}", assigned_player_id, *client_state_->client_soldier_id);
    return NetworkEventHandlerResult::Success;
}
} // namespace Soldat
