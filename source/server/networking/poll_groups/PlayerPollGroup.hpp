#ifndef __PLAYER_POLL_GROUP_HPP__
#define __PLAYER_POLL_GROUP_HPP__

#include "networking/poll_groups/PollGroupBase.hpp"
#include "networking/types/Connection.hpp"
#include "networking/events/ServerNetworkEventDispatcher.hpp"

#include <steam/steamnetworkingsockets.h>

#include <unordered_map>
#include <string>

namespace Soldat
{
class PlayerPollGroup : public PollGroupBase
{
public:
    PlayerPollGroup(ISteamNetworkingSockets* interface);

    void SetServerNetworkEventDispatcher(
      const std::shared_ptr<ServerNetworkEventDispatcher>& network_event_dispatcher);

    void PollIncomingMessages() override;
    void AcceptConnection(SteamNetConnectionStatusChangedCallback_t* new_connection_info) override;

private:
    void OnAssignConnection(const Connection& connection) override;

    std::shared_ptr<ServerNetworkEventDispatcher> network_event_dispatcher_;
};
} // namespace Soldat

#endif
