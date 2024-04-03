#ifndef __SOLDIER_INFO_NETWORK_EVENT_HANDLER_HPP__
#define __SOLDIER_INFO_NETWORK_EVENT_HANDLER_HPP__

#include "communication/NetworkPackets.hpp"
#include "core/IWorld.hpp"

#include "rendering/ClientState.hpp"

#include "communication/NetworkEventDispatcher.hpp"

namespace Soldank
{
class SoldierInfoNetworkEventHandler : public NetworkEventHandlerBase<unsigned int>
{
public:
    SoldierInfoNetworkEventHandler(const std::shared_ptr<IWorld>& world,
                                   const std::shared_ptr<ClientState>& client_state);

private:
    NetworkEvent GetTargetNetworkEvent() const override { return NetworkEvent::SoldierInfo; }

    NetworkEventHandlerResult HandleNetworkMessageImpl(unsigned int sender_connection_id,
                                                       unsigned int soldier_id) override;

    std::shared_ptr<IWorld> world_;
    std::shared_ptr<ClientState> client_state_;
};
} // namespace Soldank

#endif
