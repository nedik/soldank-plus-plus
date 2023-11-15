#ifndef __PLAYER_POLL_GROUP_HPP__
#define __PLAYER_POLL_GROUP_HPP__

#include "networking/poll_groups/IPollGroup.hpp"

#include "networking/types/Connection.hpp"

#include <steam/steamnetworkingsockets.h>

#include <unordered_map>
#include <string>

namespace Soldat
{
class PlayerPollGroup : public IPollGroup
{
public:
    PlayerPollGroup(ISteamNetworkingSockets* interface);
    ~PlayerPollGroup() override;

    PlayerPollGroup(PlayerPollGroup&& other) = delete;
    PlayerPollGroup& operator=(PlayerPollGroup&& other) = delete;
    PlayerPollGroup(PlayerPollGroup& other) = delete;
    PlayerPollGroup& operator=(PlayerPollGroup& other) = delete;

    void PollIncomingMessages() override;
    void AcceptConnection(SteamNetConnectionStatusChangedCallback_t* new_connection_info) override;
    void CloseConnection(SteamNetConnectionStatusChangedCallback_t* connection_info) override;

private:
    void SendStringToClient(HSteamNetConnection conn, const std::string& message);
    void SendStringToAllClients(const std::string& message,
                                HSteamNetConnection except = k_HSteamNetConnection_Invalid);
    void SetClientNick(HSteamNetConnection h_conn, const std::string& nick);

    HSteamNetPollGroup poll_group_handle_;

    std::unordered_map<HSteamNetConnection, Connection> player_connections_;
};
} // namespace Soldat

#endif
