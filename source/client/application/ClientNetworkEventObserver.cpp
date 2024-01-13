#include "application/ClientNetworkEventObserver.hpp"

#include <iostream>

namespace Soldat
{
ClientNetworkEventObserver::ClientNetworkEventObserver(
  const std::shared_ptr<World>& world,
  const std::shared_ptr<ClientState>& client_state)
    : world_(world)
    , client_state_(client_state)
{
}

NetworkEventObserverResult ClientNetworkEventObserver::OnAssignPlayerId(
  const ConnectionMetadata& connection_metadata,
  unsigned int assigned_player_id)
{
    const auto& soldier = world_->CreateSoldier(assigned_player_id);
    client_state_->client_soldier_id = soldier.id;
    std::cout << "OnAssignPlayerId: " << assigned_player_id << " "
              << *client_state_->client_soldier_id << std::endl;
    return NetworkEventObserverResult::Success;
}

NetworkEventObserverResult ClientNetworkEventObserver::OnChatMessage(
  const ConnectionMetadata& connection_metadata,
  const std::string& chat_message)
{
    std::cout << "OnChatMessage: " << chat_message << std::endl;
    return NetworkEventObserverResult::Success;
}
} // namespace Soldat
