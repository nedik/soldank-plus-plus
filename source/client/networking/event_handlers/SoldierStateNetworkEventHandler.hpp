#ifndef __SOLDIER_STATE_NETWORK_EVENT_HANDLER_HPP__
#define __SOLDIER_STATE_NETWORK_EVENT_HANDLER_HPP__

#include "communication/NetworkPackets.hpp"
#include "core/IWorld.hpp"

#include "rendering/ClientState.hpp"

#include "communication/NetworkEventDispatcher.hpp"

namespace Soldank
{
class SoldierStateNetworkEventHandler : public NetworkEventHandlerBase<SoldierStatePacket>
{
public:
    SoldierStateNetworkEventHandler(const std::shared_ptr<IWorld>& world,
                                    const std::shared_ptr<ClientState>& client_state);

private:
    NetworkEvent GetTargetNetworkEvent() const override { return NetworkEvent::SoldierState; }

    NetworkEventHandlerResult HandleNetworkMessageImpl(
      unsigned int sender_connection_id,
      SoldierStatePacket soldier_state_packet) override;

    std::shared_ptr<IWorld> world_;
    std::shared_ptr<ClientState> client_state_;
};
} // namespace Soldank

#endif
