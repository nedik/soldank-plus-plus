#include "networking/poll_groups/PlayerPollGroup.hpp"

#include <iostream>
#include <cassert>
#include <span>
#include <format>

namespace Soldat
{
PlayerPollGroup::PlayerPollGroup(ISteamNetworkingSockets* interface)
    : IPollGroup(interface)
    , poll_group_handle_(interface->CreatePollGroup())
{
    if (poll_group_handle_ == k_HSteamNetPollGroup_Invalid) {
        std::cout << "Failed to create a player poll group" << std::endl;
    }
}

PlayerPollGroup::~PlayerPollGroup()
{
    GetInterface()->DestroyPollGroup(poll_group_handle_);
    poll_group_handle_ = k_HSteamNetPollGroup_Invalid;
}

void PlayerPollGroup::PollIncomingMessages()
{
    while (true) {
        ISteamNetworkingMessage* incoming_message = nullptr;
        int messages_count =
          GetInterface()->ReceiveMessagesOnPollGroup(poll_group_handle_, &incoming_message, 1);
        if (messages_count == 0) {
            break;
        }
        if (messages_count < 0) {
            std::cout << "Error checking for messages" << std::endl;
        }
        assert(messages_count == 1 && incoming_message);
        auto it_client = player_connections_.find(incoming_message->m_conn);
        assert(it_client != player_connections_.end());

        std::string message_from_client;
        message_from_client.assign(static_cast<char*>(incoming_message->m_pData),
                                   incoming_message->m_cbSize);
        std::cout << std::format("[{}] {}", it_client->second.nick, message_from_client)
                  << std::endl;
        incoming_message->Release();

        std::string message_to_other_clients;
        message_to_other_clients =
          std::format("[{}] {}", it_client->second.nick.c_str(), message_from_client);
        SendStringToAllClients(message_to_other_clients, it_client->first);
    }
}

void PlayerPollGroup::AcceptConnection(
  SteamNetConnectionStatusChangedCallback_t* new_connection_info)
{
    // This must be a new connection
    assert(player_connections_.find(new_connection_info->m_hConn) == player_connections_.end());

    std::cout << "Connection request from "
              << std::span{ new_connection_info->m_info.m_szConnectionDescription }.data()
              << std::endl;

    if (GetInterface()->AcceptConnection(new_connection_info->m_hConn) != k_EResultOK) {
        GetInterface()->CloseConnection(new_connection_info->m_hConn, 0, nullptr, false);
        std::cout << "Can't accept connection. (It was already closed?)" << std::endl;
        return;
    }

    if (!GetInterface()->SetConnectionPollGroup(new_connection_info->m_hConn, poll_group_handle_)) {
        GetInterface()->CloseConnection(new_connection_info->m_hConn, 0, nullptr, false);
        std::cout << "Failed to set poll group?" << std::endl;
        return;
    }

    // TODO: ask user for their nick
    std::string nick = std::format("BraveWarrior{}", 10000 + (rand() % 100000));

    // Send them a welcome message
    std::string welcome_message = std::format("Welcome, {}", nick);
    SendStringToClient(new_connection_info->m_hConn, welcome_message);

    // Also send them a list of everybody who is already connected
    SendStringToClient(
      new_connection_info->m_hConn,
      std::format("There are {} other players on the server.", player_connections_.size()));
    for (auto& c : player_connections_) {
        SendStringToClient(new_connection_info->m_hConn, c.second.nick);
    }

    std::string message_to_other_clients = std::format("{} just joined the server.", nick);
    SendStringToAllClients(message_to_other_clients, new_connection_info->m_hConn);

    player_connections_[new_connection_info->m_hConn] = { .connection_handle =
                                                            new_connection_info->m_hConn,
                                                          .nick = nick };
    SetClientNick(new_connection_info->m_hConn, nick);
}

void PlayerPollGroup::CloseConnection(SteamNetConnectionStatusChangedCallback_t* connection_info)
{
    // Ignore if they were not previously connected. (If they disconnected
    // before we accepted the connection.)
    if (connection_info->m_eOldState == k_ESteamNetworkingConnectionState_Connected) {
        auto it_client = player_connections_.find(connection_info->m_hConn);
        assert(it_client != player_connections_.end());

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
        player_connections_.erase(it_client);
    } else {
        assert(connection_info->m_eOldState == k_ESteamNetworkingConnectionState_Connecting);
    }

    GetInterface()->CloseConnection(connection_info->m_hConn, 0, nullptr, false);
}

void PlayerPollGroup::SendStringToClient(HSteamNetConnection conn, const std::string& message)
{
    GetInterface()->SendMessageToConnection(
      conn, message.c_str(), message.size(), k_nSteamNetworkingSend_Reliable, nullptr);
}

void PlayerPollGroup::SendStringToAllClients(const std::string& message, HSteamNetConnection except)
{
    for (auto& c : player_connections_) {
        if (c.first != except) {
            SendStringToClient(c.first, message);
        }
    }
}

void PlayerPollGroup::SetClientNick(HSteamNetConnection h_conn, const std::string& nick)
{
    player_connections_[h_conn].nick = nick;
    // Set the connection name, too, which is useful for debugging
    GetInterface()->SetConnectionName(h_conn, nick.c_str());
}
} // namespace Soldat
