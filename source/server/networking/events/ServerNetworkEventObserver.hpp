#ifndef __SERVER_NETWORK_EVENT_OBSERVER_HPP__
#define __SERVER_NETWORK_EVENT_OBSERVER_HPP__

#include "communication/NetworkEventDispatcher.hpp"

#include "core/IWorld.hpp"

namespace Soldat
{
class ServerNetworkEventObserver : public INetworkEventObserver
{
public:
    ServerNetworkEventObserver(const std::shared_ptr<IWorld>& world);

    NetworkEventObserverResult OnAssignPlayerId(const ConnectionMetadata& connection_metadata,
                                                unsigned int assigned_player_id) override;
    NetworkEventObserverResult OnChatMessage(const ConnectionMetadata& connection_metadata,
                                             const std::string& chat_message) override;
    NetworkEventObserverResult OnSpawnSoldier(const ConnectionMetadata& connection_metadata,
                                              unsigned int soldier_id,
                                              glm::vec2 spawn_position) override;

    unsigned int OnCreateNewSoldier();
    glm::vec2 SpawnSoldier(unsigned int soldier_id);

private:
    std::shared_ptr<IWorld> world_;
};
} // namespace Soldat

#endif
