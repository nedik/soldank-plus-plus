#include "networking/events/ServerNetworkEventObserver.hpp"
#include "communication/NetworkEventDispatcher.hpp"
#include "communication/NetworkMessage.hpp"

#include "spdlog/spdlog.h"

namespace Soldat
{
ServerNetworkEventObserver::ServerNetworkEventObserver(const std::shared_ptr<IWorld>& world)
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
    spdlog::info("OnChatMessage: {}", chat_message);
    return NetworkEventObserverResult::Success;
}

NetworkEventObserverResult ServerNetworkEventObserver::OnSpawnSoldier(
  const ConnectionMetadata& connection_metadata,
  unsigned int soldier_id,
  glm::vec2 spawn_position)
{
    return NetworkEventObserverResult::Success;
}

NetworkEventObserverResult ServerNetworkEventObserver::OnUpdateSoldierState(
  const ConnectionMetadata& connection_metadata,
  unsigned int soldier_id,
  glm::vec2 soldier_position)
{
    return NetworkEventObserverResult::Success;
}
} // namespace Soldat
