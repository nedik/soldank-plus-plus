#include "networking/poll_groups/PollGroupBase.hpp"

#include <iostream>
#include <cassert>
#include <format>
#include <utility>

namespace Soldat
{
PollGroupBase::PollGroupBase(ISteamNetworkingSockets* interface)
    : IPollGroup(interface)
    , poll_group_handle_(interface->CreatePollGroup())
{
    if (poll_group_handle_ == k_HSteamNetPollGroup_Invalid) {
        std::cout << "Failed to create a player poll group" << std::endl;
    }
}

PollGroupBase::~PollGroupBase()
{
    GetInterface()->DestroyPollGroup(poll_group_handle_);
    poll_group_handle_ = k_HSteamNetPollGroup_Invalid;
}

void PollGroupBase::CloseConnection(SteamNetConnectionStatusChangedCallback_t* connection_info)
{
    // Ignore if they were not previously connected. (If they disconnected
    // before we accepted the connection.)
    if (connection_info->m_eOldState == k_ESteamNetworkingConnectionState_Connected) {
        auto it_client = connections_.find(connection_info->m_hConn);
        assert(it_client != connections_.end());

        std::string debug_log_action;
        if (connection_info->m_info.m_eState ==
            k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
            debug_log_action = "problem detected locally";
        } else {
            debug_log_action = "closed by peer";
        }

        std::cout << std::format("Connection {} {}, reason {}: {}",
                                 connection_info->m_info.m_szConnectionDescription,
                                 debug_log_action,
                                 connection_info->m_info.m_eEndReason,
                                 connection_info->m_info.m_szEndDebug)
                  << std::endl;

        SendStringToAllClients(std::format("{} {}", it_client->second.nick, "has left the server"),
                               it_client->first);
        connections_.erase(it_client);
    } else {
        assert(connection_info->m_eOldState == k_ESteamNetworkingConnectionState_Connecting);
    }

    GetInterface()->CloseConnection(connection_info->m_hConn, 0, nullptr, false);
}

bool PollGroupBase::AssignConnection(const Connection& connection)
{
    if (!GetInterface()->SetConnectionPollGroup(connection.connection_handle, poll_group_handle_)) {
        GetInterface()->CloseConnection(connection.connection_handle, 0, nullptr, false);
        std::cout << "Failed to set poll group?" << std::endl;
        return false;
    }
    connections_[connection.connection_handle] = connection;
    OnAssignConnection(connection);

    return true;
}

void PollGroupBase::SendNetworkMessage(HSteamNetConnection connection_id,
                                       const NetworkMessage& network_message)
{
    GetInterface()->SendMessageToConnection(connection_id,
                                            network_message.GetData().data(),
                                            network_message.GetData().size(),
                                            k_nSteamNetworkingSend_Reliable,
                                            nullptr);
}

void PollGroupBase::SendNetworkMessageToAll(const NetworkMessage& network_message)
{
    for (auto& connection : connections_) {
        GetInterface()->SendMessageToConnection(connection.second.connection_handle,
                                                network_message.GetData().data(),
                                                network_message.GetData().size(),
                                                k_nSteamNetworkingSend_Reliable,
                                                nullptr);
    }
}

void PollGroupBase::OnAssignConnection(const Connection& /* connection */) {}

bool PollGroupBase::IsConnectionAssigned(HSteamNetConnection steam_net_connection_handle)
{
    auto it_client = connections_.find(steam_net_connection_handle);
    return it_client != connections_.end();
}

HSteamNetPollGroup PollGroupBase::GetPollGroupHandle() const
{
    return poll_group_handle_;
}

std::unordered_map<HSteamNetConnection, Connection>::iterator PollGroupBase::FindConnection(
  HSteamNetConnection steam_net_connection_handle)
{
    return connections_.find(steam_net_connection_handle);
}

void PollGroupBase::EraseConnection(
  std::unordered_map<HSteamNetConnection, Connection>::iterator it_connection)
{
    connections_.erase(std::move(it_connection));
}

void PollGroupBase::SendStringToClient(HSteamNetConnection conn, const std::string& message)
{
    GetInterface()->SendMessageToConnection(
      conn, message.c_str(), message.size(), k_nSteamNetworkingSend_Reliable, nullptr);
}

void PollGroupBase::SendStringToAllClients(const std::string& message, HSteamNetConnection except)
{
    for (auto& c : connections_) {
        if (c.first != except) {
            SendStringToClient(c.first, message);
        }
    }
}

void PollGroupBase::SetClientNick(HSteamNetConnection h_conn, const std::string& nick)
{
    connections_[h_conn].nick = nick;
    // Set the connection name, too, which is useful for debugging
    GetInterface()->SetConnectionName(h_conn, nick.c_str());
}
} // namespace Soldat
