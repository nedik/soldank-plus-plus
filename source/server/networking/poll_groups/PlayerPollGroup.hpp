#ifndef __PLAYER_POLL_GROUP_HPP__
#define __PLAYER_POLL_GROUP_HPP__

#include "networking/poll_groups/PollGroupBase.hpp"
#include "networking/types/Connection.hpp"

#include "core/IWorld.hpp"
#include "communication/NetworkEventDispatcher.hpp"

#include <steam/steamnetworkingsockets.h>

#include <unordered_map>
#include <string>
#include <memory>

namespace Soldat
{
class PlayerPollGroup : public PollGroupBase
{
public:
    PlayerPollGroup(ISteamNetworkingSockets* interface);

    void SetServerNetworkEventDispatcher(
      const std::shared_ptr<NetworkEventDispatcher>& network_event_dispatcher);
    void SetWorld(const std::shared_ptr<IWorld>& world);

    void PollIncomingMessages() override;
    void AcceptConnection(SteamNetConnectionStatusChangedCallback_t* new_connection_info) override;

private:
    void OnAssignConnection(const Connection& connection) override;

    std::shared_ptr<NetworkEventDispatcher> network_event_dispatcher_;
    std::shared_ptr<IWorld> world_;
};
} // namespace Soldat

#endif
