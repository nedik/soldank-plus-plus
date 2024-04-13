#ifndef __SOLDIER_INFO_NETWORK_EVENT_HANDLER_HPP__
#define __SOLDIER_INFO_NETWORK_EVENT_HANDLER_HPP__

#include "communication/NetworkPackets.hpp"
#include "core/IWorld.hpp"

#include "rendering/ClientState.hpp"

#include "communication/NetworkEventDispatcher.hpp"

namespace Soldank
{
class SoldierInfoNetworkEventHandler : public NetworkEventHandlerBase<std::uint8_t, std::string>
{
public:
    SoldierInfoNetworkEventHandler(const std::shared_ptr<IWorld>& world,
                                   const std::shared_ptr<ClientState>& client_state);

private:
    NetworkEvent GetTargetNetworkEvent() const override { return NetworkEvent::SoldierInfo; }

    NetworkEventHandlerResult HandleNetworkMessageImpl(unsigned int sender_connection_id,
                                                       std::uint8_t soldier_id,
                                                       std::string player_nick) override;

    std::shared_ptr<IWorld> world_;
    std::shared_ptr<ClientState> client_state_;
};
} // namespace Soldank

#endif
