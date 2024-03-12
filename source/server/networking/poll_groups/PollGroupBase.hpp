#ifndef __POLL_GROUP_BASE_HPP__
#define __POLL_GROUP_BASE_HPP__

#include "networking/poll_groups/IPollGroup.hpp"

#include <unordered_map>

namespace Soldat
{
class PollGroupBase : public IPollGroup
{
public:
    PollGroupBase(ISteamNetworkingSockets* interface);
    ~PollGroupBase() override;

    PollGroupBase(PollGroupBase&& other) = delete;
    PollGroupBase& operator=(PollGroupBase&& other) = delete;
    PollGroupBase(PollGroupBase& other) = delete;
    PollGroupBase& operator=(PollGroupBase& other) = delete;

    void CloseConnection(SteamNetConnectionStatusChangedCallback_t* connection_info) override;
    bool AssignConnection(const Connection& connection) override;
    bool IsConnectionAssigned(HSteamNetConnection steam_net_connection_handle) override;

    void SendNetworkMessage(HSteamNetConnection connection_id,
                            const NetworkMessage& network_message) override;
    void SendNetworkMessageToAll(
      const NetworkMessage& network_message,
      std::optional<unsigned int> except_connection_id = std::nullopt) override;

    void SendReliableNetworkMessage(unsigned int connection_id,
                                    const NetworkMessage& network_message) override;
    void SendReliableNetworkMessageToAll(
      const NetworkMessage& network_message,
      std::optional<unsigned int> except_connection_id = std::nullopt) override;

protected:
    virtual void OnAssignConnection(const Connection& connection);

    HSteamNetPollGroup GetPollGroupHandle() const;

    std::unordered_map<HSteamNetConnection, Connection>::iterator FindConnection(
      HSteamNetConnection steam_net_connection_handle);
    void EraseConnection(
      std::unordered_map<HSteamNetConnection, Connection>::iterator it_connection);

    void SendStringToClient(HSteamNetConnection conn, const std::string& message);
    void SendStringToAllClients(const std::string& message,
                                HSteamNetConnection except = k_HSteamNetConnection_Invalid);
    void SetClientNick(HSteamNetConnection h_conn, const std::string& nick);

private:
    HSteamNetPollGroup poll_group_handle_;

    std::unordered_map<HSteamNetConnection, Connection> connections_;
};
} // namespace Soldat

#endif
