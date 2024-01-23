#include "application/ClientNetworkEventObserver.hpp"
#include "communication/NetworkEventDispatcher.hpp"

#include "spdlog/spdlog.h"

namespace Soldat
{
ClientNetworkEventObserver::ClientNetworkEventObserver(
  const std::shared_ptr<IWorld>& world,
  const std::shared_ptr<ClientState>& client_state)
    : world_(world)
    , client_state_(client_state)
{
}

NetworkEventObserverResult ClientNetworkEventObserver::OnAssignPlayerId(
  const ConnectionMetadata& /*connection_metadata*/,
  unsigned int assigned_player_id)
{
    const auto& soldier = world_->CreateSoldier(assigned_player_id);
    client_state_->client_soldier_id = soldier.id;
    spdlog::info("OnAssignPlayerId: {} {}", assigned_player_id, *client_state_->client_soldier_id);
    return NetworkEventObserverResult::Success;
}

NetworkEventObserverResult ClientNetworkEventObserver::OnChatMessage(
  const ConnectionMetadata& /*connection_metadata*/,
  const std::string& chat_message)
{
    spdlog::info("OnChatMessage: {}", chat_message);
    return NetworkEventObserverResult::Success;
}

NetworkEventObserverResult ClientNetworkEventObserver::OnSpawnSoldier(
  const ConnectionMetadata& /*connection_metadata*/,
  unsigned int soldier_id,
  glm::vec2 spawn_position)
{
    spdlog::info("OnSpawnSoldier: {}, ({}, {})", soldier_id, spawn_position.x, spawn_position.y);
    world_->SpawnSoldier(soldier_id, spawn_position);
    return NetworkEventObserverResult::Success;
}

NetworkEventObserverResult ClientNetworkEventObserver::OnSoldierInput(
  const ConnectionMetadata& connection_metadata,
  unsigned int soldier_id,
  glm::vec2 soldier_position,
  const Control& player_control)
{
    return NetworkEventObserverResult::Success;
}

NetworkEventObserverResult ClientNetworkEventObserver::OnSoldierState(
  const ConnectionMetadata& connection_metadata,
  unsigned int soldier_id,
  glm::vec2 soldier_position)
{
    client_state_->soldier_position_server_pov = { soldier_position.x, soldier_position.y };
    auto state = world_->GetState();
    for (auto& soldier : state->soldiers) {
        if (soldier.id == soldier_id) {
            auto diff = soldier_position - soldier.particle.position;
            soldier.particle.position = soldier_position;
            soldier.particle.old_position += diff;
        }
    }
    return NetworkEventObserverResult::Success;
}
} // namespace Soldat
