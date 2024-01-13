#ifndef __CLIENT_NETWORK_EVENT_OBSERVER_HPP__
#define __CLIENT_NETWORK_EVENT_OBSERVER_HPP__

#include "communication/NetworkEventDispatcher.hpp"

#include "core/World.hpp"

#include "rendering/ClientState.hpp"

#include <memory>

namespace Soldat
{
class ClientNetworkEventObserver : public INetworkEventObserver
{
public:
    ClientNetworkEventObserver(const std::shared_ptr<World>& world,
                               const std::shared_ptr<ClientState>& client_state);

    NetworkEventObserverResult OnAssignPlayerId(const ConnectionMetadata& connection_metadata,
                                                unsigned int assigned_player_id) override;
    NetworkEventObserverResult OnChatMessage(const ConnectionMetadata& connection_metadata,
                                             const std::string& chat_message) override;

private:
    std::shared_ptr<World> world_;
    std::shared_ptr<ClientState> client_state_;
};
} // namespace Soldat

#endif
