#ifndef __SPAWN_SOLDIER_NETWORK_EVENT_HANDLER_HPP__
#define __SPAWN_SOLDIER_NETWORK_EVENT_HANDLER_HPP__

#include "core/IWorld.hpp"

#include "communication/NetworkEventDispatcher.hpp"

namespace Soldat
{
class SpawnSoldierNetworkEventHandler : public NetworkEventHandlerBase<unsigned int, float, float>
{
public:
    SpawnSoldierNetworkEventHandler(const std::shared_ptr<IWorld>& world);

private:
    NetworkEvent GetTargetNetworkEvent() const override { return NetworkEvent::SpawnSoldier; }

    NetworkEventHandlerResult HandleNetworkMessageImpl(unsigned int sender_connection_id,
                                                       unsigned int soldier_id,
                                                       float spawn_position_x,
                                                       float spawn_position_y) override;

    std::shared_ptr<IWorld> world_;
};
} // namespace Soldat

#endif
