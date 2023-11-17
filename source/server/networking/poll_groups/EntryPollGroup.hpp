#ifndef __ENTRY_POLL_GROUP_HPP__
#define __ENTRY_POLL_GROUP_HPP__

#include "networking/poll_groups/PollGroupBase.hpp"
#include "networking/types/Connection.hpp"

#include <steam/steamnetworkingsockets.h>

#include <unordered_map>
#include <string>
#include <memory>

namespace Soldat
{
class EntryPollGroup : public PollGroupBase
{
public:
    EntryPollGroup(ISteamNetworkingSockets* interface);

    void PollIncomingMessages() override;
    void AcceptConnection(SteamNetConnectionStatusChangedCallback_t* new_connection_info) override;

    void RegisterPlayerPollGroup(std::shared_ptr<IPollGroup> poll_group);

private:
    std::shared_ptr<IPollGroup> player_poll_group_;
};
} // namespace Soldat

#endif
