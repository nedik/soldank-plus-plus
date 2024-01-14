#include "networking/events/ServerNetworkEventDispatcher.hpp"

namespace Soldat
{
ServerNetworkEventDispatcher::ServerNetworkEventDispatcher(
  const std::shared_ptr<ServerNetworkEventObserver>& server_network_event_observer)
    : NetworkEventDispatcher(server_network_event_observer)
    , server_network_event_observer_(server_network_event_observer)
{
}

unsigned int ServerNetworkEventDispatcher::CreateNewSoldier()
{
    return server_network_event_observer_->OnCreateNewSoldier();
}

glm::vec2 ServerNetworkEventDispatcher::SpawnSoldier(unsigned int soldier_id)
{
    return server_network_event_observer_->SpawnSoldier(soldier_id);
}
} // namespace Soldat
