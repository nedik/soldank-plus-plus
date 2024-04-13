#ifndef __ASSIGN_PLAYER_ID_NETWORK_EVENT_HANDLER_HPP__
#define __ASSIGN_PLAYER_ID_NETWORK_EVENT_HANDLER_HPP__

#include "core/IWorld.hpp"

#include "rendering/ClientState.hpp"

#include "communication/NetworkEventDispatcher.hpp"

namespace Soldank
{
class AssignPlayerIdNetworkEventHandler : public NetworkEventHandlerBase<std::uint8_t>
{
public:
    AssignPlayerIdNetworkEventHandler(const std::shared_ptr<IWorld>& world,
                                      const std::shared_ptr<ClientState>& client_state);

private:
    NetworkEvent GetTargetNetworkEvent() const override { return NetworkEvent::AssignPlayerId; }

    NetworkEventHandlerResult HandleNetworkMessageImpl(unsigned int sender_connection_id,
                                                       std::uint8_t assigned_player_id) override;

    std::shared_ptr<IWorld> world_;
    std::shared_ptr<ClientState> client_state_;
};
} // namespace Soldank

#endif
