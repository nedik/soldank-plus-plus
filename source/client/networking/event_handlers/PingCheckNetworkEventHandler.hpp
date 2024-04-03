#ifndef __PING_CHECK_NETWORK_EVENT_HANDLER_HPP__
#define __PING_CHECK_NETWORK_EVENT_HANDLER_HPP__

#include "rendering/ClientState.hpp"

#include "communication/NetworkEventDispatcher.hpp"

namespace Soldank
{
class PingCheckNetworkEventHandler : public NetworkEventHandlerBase<>
{
public:
    PingCheckNetworkEventHandler(const std::shared_ptr<ClientState>& client_state);

private:
    NetworkEvent GetTargetNetworkEvent() const override { return NetworkEvent::PingCheck; }

    NetworkEventHandlerResult HandleNetworkMessageImpl(unsigned int sender_connection_id) override;

    std::shared_ptr<ClientState> client_state_;
};
} // namespace Soldank

#endif
