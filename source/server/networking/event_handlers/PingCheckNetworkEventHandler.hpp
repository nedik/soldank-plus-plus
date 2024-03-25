#ifndef __PING_CHECK_NETWORK_EVENT_HANDLER_HPP__
#define __PING_CHECK_NETWORK_EVENT_HANDLER_HPP__

#include "networking/IGameServer.hpp"

#include "communication/NetworkEventDispatcher.hpp"

#include <memory>

namespace Soldat
{
class PingCheckNetworkEventHandler : public NetworkEventHandlerBase<>
{
public:
    PingCheckNetworkEventHandler(const std::shared_ptr<IGameServer>& game_server);

private:
    NetworkEvent GetTargetNetworkEvent() const override { return NetworkEvent::PingCheck; }

    NetworkEventHandlerResult HandleNetworkMessageImpl(unsigned int sender_connection_id) override;

    std::shared_ptr<IGameServer> game_server_;
};
} // namespace Soldat

#endif
