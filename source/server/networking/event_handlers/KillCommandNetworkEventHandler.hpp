#ifndef __KILL_COMMAND_NETWORK_EVENT_HANDLER_HPP__
#define __KILL_COMMAND_NETWORK_EVENT_HANDLER_HPP__

#include "networking/IGameServer.hpp"

#include "communication/NetworkEventDispatcher.hpp"

#include "core/IWorld.hpp"

#include <memory>

namespace Soldank
{
class KillCommandNetworkEventHandler : public NetworkEventHandlerBase<>
{
public:
    KillCommandNetworkEventHandler(const std::shared_ptr<IWorld>& world,
                                   const std::shared_ptr<IGameServer>& game_server);

private:
    NetworkEvent GetTargetNetworkEvent() const override { return NetworkEvent::KillCommand; }

    NetworkEventHandlerResult HandleNetworkMessageImpl(unsigned int sender_connection_id) override;

    std::shared_ptr<IWorld> world_;
    std::shared_ptr<IGameServer> game_server_;
};
} // namespace Soldank

#endif
