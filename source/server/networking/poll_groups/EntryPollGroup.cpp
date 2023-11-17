#include "networking/poll_groups/EntryPollGroup.hpp"

#include <iostream>
#include <utility>
#include <span>
#include <cassert>
#include <format>

namespace Soldat
{
EntryPollGroup::EntryPollGroup(ISteamNetworkingSockets* interface)
    : PollGroupBase(interface)
{
}

void EntryPollGroup::PollIncomingMessages()
{
    while (true) {
        ISteamNetworkingMessage* incoming_message = nullptr;
        int messages_count =
          GetInterface()->ReceiveMessagesOnPollGroup(GetPollGroupHandle(), &incoming_message, 1);
        if (messages_count == 0) {
            break;
        }
        if (messages_count < 0) {
            std::cout << "[EntryPollGroup] Error checking for messages" << std::endl;
        }
        assert(messages_count == 1 && incoming_message);
        assert(IsConnectionAssigned(incoming_message->m_conn));
        auto it_client = FindConnection(incoming_message->m_conn);

        std::string message_from_client;
        message_from_client.assign(static_cast<char*>(incoming_message->m_pData),
                                   incoming_message->m_cbSize);
        it_client->second.nick = message_from_client;
        std::cout << std::format("[EntryPollGroup] Name assigned to connection {}: {}",
                                 it_client->second.connection_handle,
                                 it_client->second.nick)
                  << std::endl;
        incoming_message->Release();

        player_poll_group_->AssignConnection(it_client->second);
        EraseConnection(it_client);
    }
}

void EntryPollGroup::AcceptConnection(
  SteamNetConnectionStatusChangedCallback_t* new_connection_info)
{
    // This must be a new connection
    assert(!IsConnectionAssigned(new_connection_info->m_hConn));

    std::cout << "[EntryPollGroup] Connection request from "
              << std::span{ new_connection_info->m_info.m_szConnectionDescription }.data()
              << std::endl;

    if (GetInterface()->AcceptConnection(new_connection_info->m_hConn) != k_EResultOK) {
        GetInterface()->CloseConnection(new_connection_info->m_hConn, 0, nullptr, false);
        std::cout << "[EntryPollGroup] Can't accept connection. (It was already closed?)"
                  << std::endl;
        return;
    }

    std::cout << std::format(
      "[EntryPollGroup] Connection accepted: {}\n",
      std::span{ new_connection_info->m_info.m_szConnectionDescription }.data());

    if (!AssignConnection({ new_connection_info->m_hConn, "NEW CONNECTION PLACEHOLDER" })) {
        return;
    }

    std::cout << std::format(
                   "[EntryPollGroup] Connection assigned: {}",
                   std::span{ new_connection_info->m_info.m_szConnectionDescription }.data())
              << std::endl;
}

void EntryPollGroup::RegisterPlayerPollGroup(std::shared_ptr<IPollGroup> poll_group)
{
    player_poll_group_ = std::move(poll_group);
}

} // namespace Soldat