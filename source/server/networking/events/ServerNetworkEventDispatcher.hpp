#ifndef __SERVER_NETWORK_EVENT_DISPATCHER_HPP__
#define __SERVER_NETWORK_EVENT_DISPATCHER_HPP__

#include "networking/events/ServerNetworkEventObserver.hpp"

#include "communication/NetworkEventDispatcher.hpp"

namespace Soldat
{
class ServerNetworkEventDispatcher : public NetworkEventDispatcher
{
public:
    ServerNetworkEventDispatcher(
      const std::shared_ptr<ServerNetworkEventObserver>& server_network_event_observer);

    unsigned int CreateNewSoldier();

private:
    std::shared_ptr<ServerNetworkEventObserver> server_network_event_observer_;
};
} // namespace Soldat

#endif
