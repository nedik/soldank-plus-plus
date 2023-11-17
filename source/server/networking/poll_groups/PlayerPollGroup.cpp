#include "networking/poll_groups/PlayerPollGroup.hpp"
#include "networking/poll_groups/PollGroupBase.hpp"

#include <iostream>
#include <cassert>
#include <span>
#include <format>

namespace Soldat
{
PlayerPollGroup::PlayerPollGroup(ISteamNetworkingSockets* interface)
    : PollGroupBase(interface)
{
}

void PlayerPollGroup::PollIncomingMessages()
{
    while (true) {
        ISteamNetworkingMessage* incoming_message = nullptr;
        int messages_count =
          GetInterface()->ReceiveMessagesOnPollGroup(GetPollGroupHandle(), &incoming_message, 1);
        if (messages_count == 0) {
            break;
        }
        if (messages_count < 0) {
            std::cout << "Error checking for messages" << std::endl;
        }
        assert(messages_count == 1 && incoming_message);
        assert(IsConnectionAssigned(incoming_message->m_conn));
        auto it_client = FindConnection(incoming_message->m_conn);

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
  SteamNetConnectionStatusChangedCallback_t* /*new_connection_info*/)
{
}
} // namespace Soldat
