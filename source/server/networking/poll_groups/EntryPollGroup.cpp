#include "networking/poll_groups/EntryPollGroup.hpp"

#include "spdlog/spdlog.h"

#include <utility>
#include <span>
#include <cassert>

namespace Soldank
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
            spdlog::error("[EntryPollGroup] Error checking for messages");
        }
        assert(messages_count == 1 && incoming_message);
        assert(IsConnectionAssigned(incoming_message->m_conn));
        auto it_client = FindConnection(incoming_message->m_conn);

        std::string message_from_client;
        message_from_client.assign(static_cast<char*>(incoming_message->m_pData),
                                   incoming_message->m_cbSize);
        it_client->second.nick = message_from_client;
        SetClientNick(it_client->second.connection_handle, it_client->second.nick);
        spdlog::info("[EntryPollGroup] Name assigned to connection {}: {}",
                     it_client->second.connection_handle,
                     it_client->second.nick);
        incoming_message->Release();

        SendNetworkMessage(
          it_client->second.connection_handle,
          { NetworkEvent::ChatMessage, "Welcome to the server " + it_client->second.nick });

        player_poll_group_->AssignConnection(it_client->second);
        EraseConnection(it_client);
    }
}

void EntryPollGroup::AcceptConnection(
  SteamNetConnectionStatusChangedCallback_t* new_connection_info)
{
    // This must be a new connection
    assert(!IsConnectionAssigned(new_connection_info->m_hConn));

    spdlog::info("[EntryPollGroup] Connection request from {}",
                 std::span{ new_connection_info->m_info.m_szConnectionDescription }.data());

    if (GetInterface()->AcceptConnection(new_connection_info->m_hConn) != k_EResultOK) {
        GetInterface()->CloseConnection(new_connection_info->m_hConn, 0, nullptr, false);
        spdlog::warn("[EntryPollGroup] Can't accept connection. (It was already closed?)");
        return;
    }

    spdlog::info("[EntryPollGroup] Connection accepted: {}\n",
                 std::span{ new_connection_info->m_info.m_szConnectionDescription }.data());

    if (!AssignConnection({ .connection_handle = new_connection_info->m_hConn,
                            .nick = "NEW CONNECTION PLACEHOLDER" })) {
        return;
    }

    spdlog::info("[EntryPollGroup] Connection assigned: {}",
                 std::span{ new_connection_info->m_info.m_szConnectionDescription }.data());

    SetClientNick(new_connection_info->m_hConn, "NEW CONNECTION PLACEHOLDER");
}

void EntryPollGroup::RegisterPlayerPollGroup(std::shared_ptr<IPollGroup> poll_group)
{
    player_poll_group_ = std::move(poll_group);
}

} // namespace Soldank