#ifndef __PROJECTILE_SPAWN_NETWORK_EVENT_HANDLER_HPP__
#define __PROJECTILE_SPAWN_NETWORK_EVENT_HANDLER_HPP__

#include "communication/NetworkPackets.hpp"
#include "core/IWorld.hpp"

#include "communication/NetworkEventDispatcher.hpp"

namespace Soldank
{
class ProjectileSpawnNetworkEventHandler : public NetworkEventHandlerBase<ProjectileSpawnPacket>
{
public:
    ProjectileSpawnNetworkEventHandler(const std::shared_ptr<IWorld>& world);

private:
    NetworkEvent GetTargetNetworkEvent() const override { return NetworkEvent::ProjectileSpawn; }

    NetworkEventHandlerResult HandleNetworkMessageImpl(
      unsigned int sender_connection_id,
      ProjectileSpawnPacket projectile_spawn_packet) override;

    std::shared_ptr<IWorld> world_;
};
} // namespace Soldank

#endif
