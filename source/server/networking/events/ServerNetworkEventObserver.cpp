#include "networking/events/ServerNetworkEventObserver.hpp"
#include "communication/NetworkEventDispatcher.hpp"
#include "communication/NetworkMessage.hpp"

#include <iostream>

namespace Soldat
{
ServerNetworkEventObserver::ServerNetworkEventObserver(const std::shared_ptr<World>& world)
    : world_(world)
{
}

NetworkEventObserverResult ServerNetworkEventObserver::OnAssignPlayerId(
  const ConnectionMetadata& connection_metadata,
  unsigned int assigned_player_id)
{
    // TODO: this is not used
    return NetworkEventObserverResult::Success;
}

NetworkEventObserverResult ServerNetworkEventObserver::OnChatMessage(
  const ConnectionMetadata& connection_metadata,
  const std::string& chat_message)
{
    std::cout << "OnChatMessage: " << chat_message << std::endl;
    return NetworkEventObserverResult::Success;
}

unsigned int ServerNetworkEventObserver::OnCreateNewSoldier()
{
    const auto& soldier = world_->CreateSoldier();
    std::cout << "OnCreateSoldier: " << soldier.id << std::endl;
    return soldier.id;
}

NetworkEventObserverResult ServerNetworkEventObserver::OnSpawnSoldier(
  const ConnectionMetadata& connection_metadata,
  unsigned int soldier_id,
  glm::vec2 spawn_position)
{
    return NetworkEventObserverResult::Success;
}

glm::vec2 ServerNetworkEventObserver::SpawnSoldier(unsigned int soldier_id)
{
    return world_->SpawnSoldier(soldier_id);
}
} // namespace Soldat
