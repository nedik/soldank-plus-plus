#ifndef __SOLDIER_INPUT_NETWORK_EVENT_HANDLER_HPP__
#define __SOLDIER_INPUT_NETWORK_EVENT_HANDLER_HPP__

#include "communication/NetworkEventDispatcher.hpp"
#include "communication/NetworkPackets.hpp"

#include "application/ServerState.hpp"

#include "core/IWorld.hpp"

#include <memory>

namespace Soldat
{
class SoldierInputNetworkEventHandler : public NetworkEventHandlerBase<SoldierInputPacket>
{
public:
    SoldierInputNetworkEventHandler(const std::shared_ptr<IWorld>& world,
                                    const std::shared_ptr<ServerState>& server_state);

private:
    NetworkEvent GetTargetNetworkEvent() const override { return NetworkEvent::SoldierInput; }

    NetworkEventHandlerResult HandleNetworkMessageImpl(
      unsigned int sender_connection_id,
      SoldierInputPacket soldier_input_packet) override;

    std::shared_ptr<IWorld> world_;
    std::shared_ptr<ServerState> server_state_;
};
} // namespace Soldat

#endif
