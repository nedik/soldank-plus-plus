#ifndef __KILL_SOLDIER_NETWORK_EVENT_HANDLER_HPP__
#define __KILL_SOLDIER_NETWORK_EVENT_HANDLER_HPP__

#include "core/IWorld.hpp"

#include "communication/NetworkEventDispatcher.hpp"

namespace Soldank
{
class KillSoldierNetworkEventHandler : public NetworkEventHandlerBase<unsigned int>
{
public:
    KillSoldierNetworkEventHandler(const std::shared_ptr<IWorld>& world);

private:
    NetworkEvent GetTargetNetworkEvent() const override { return NetworkEvent::KillSoldier; }

    NetworkEventHandlerResult HandleNetworkMessageImpl(unsigned int sender_connection_id,
                                                       unsigned int soldier_id) override;

    std::shared_ptr<IWorld> world_;
};
} // namespace Soldank

#endif
