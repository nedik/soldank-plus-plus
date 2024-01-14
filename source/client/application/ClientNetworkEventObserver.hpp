#ifndef __CLIENT_NETWORK_EVENT_OBSERVER_HPP__
#define __CLIENT_NETWORK_EVENT_OBSERVER_HPP__

#include "communication/NetworkEventDispatcher.hpp"

#include "core/IWorld.hpp"

#include "rendering/ClientState.hpp"

#include <memory>

namespace Soldat
{
class ClientNetworkEventObserver : public INetworkEventObserver
{
public:
    ClientNetworkEventObserver(const std::shared_ptr<IWorld>& world,
                               const std::shared_ptr<ClientState>& client_state);

    NetworkEventObserverResult OnAssignPlayerId(const ConnectionMetadata& connection_metadata,
                                                unsigned int assigned_player_id) override;
    NetworkEventObserverResult OnChatMessage(const ConnectionMetadata& connection_metadata,
                                             const std::string& chat_message) override;
    NetworkEventObserverResult OnSpawnSoldier(const ConnectionMetadata& connection_metadata,
                                              unsigned int soldier_id,
                                              glm::vec2 spawn_position) override;

private:
    std::shared_ptr<IWorld> world_;
    std::shared_ptr<ClientState> client_state_;
};
} // namespace Soldat

#endif
