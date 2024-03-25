#ifndef __PLAYER_LEAVE_NETWORK_EVENT_HANDLER_HPP__
#define __PLAYER_LEAVE_NETWORK_EVENT_HANDLER_HPP__

#include "core/IWorld.hpp"

#include "communication/NetworkEventDispatcher.hpp"

namespace Soldat
{
class PlayerLeaveNetworkEventHandler : public NetworkEventHandlerBase<unsigned int>
{
public:
    PlayerLeaveNetworkEventHandler(const std::shared_ptr<IWorld>& world);

private:
    NetworkEvent GetTargetNetworkEvent() const override { return NetworkEvent::PlayerLeave; }

    NetworkEventHandlerResult HandleNetworkMessageImpl(unsigned int sender_connection_id,
                                                       unsigned int soldier_id) override;

    std::shared_ptr<IWorld> world_;
};
} // namespace Soldat

#endif
