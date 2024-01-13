#include "networking/poll_groups/PlayerPollGroup.hpp"
#include "communication/NetworkEventDispatcher.hpp"
#include "networking/poll_groups/PollGroupBase.hpp"

#include "communication/NetworkMessage.hpp"

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

void PlayerPollGroup::SetServerNetworkEventDispatcher(
  const std::shared_ptr<ServerNetworkEventDispatcher>& network_event_dispatcher)
{
    network_event_dispatcher_ = network_event_dispatcher;
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
            break;
        }
        assert(messages_count == 1 && incoming_message);
        assert(IsConnectionAssigned(incoming_message->m_conn));

        auto it_client = FindConnection(incoming_message->m_conn);

        std::span<char> received_bytes{ static_cast<char*>(incoming_message->m_pData),
                                        static_cast<unsigned int>(incoming_message->m_cbSize) };
        incoming_message->Release();
        NetworkMessage network_message(received_bytes);

        ConnectionMetadata connection_metadata{ .connection_id = incoming_message->m_conn,
                                                .send_message_to_connection =
                                                  [](const NetworkMessage& message) { // TODO
                                                  } };
        network_event_dispatcher_->ProcessNetworkMessage(connection_metadata, network_message);
    }
}

void PlayerPollGroup::AcceptConnection(
  SteamNetConnectionStatusChangedCallback_t* /*new_connection_info*/)
{
}

void PlayerPollGroup::OnAssignConnection(const Connection& connection)
{
    unsigned int soldier_id = network_event_dispatcher_->CreateNewSoldier();
    std::cout << "OnAssignPlayerId: " << soldier_id << std::endl;
    // TODO: send id back to the client
    NetworkMessage network_message(NetworkEvent::AssignPlayerId, soldier_id);
    SendNetworkMessage(connection.connection_handle, network_message);
}
} // namespace Soldat
