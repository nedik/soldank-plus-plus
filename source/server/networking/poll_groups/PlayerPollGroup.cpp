#include "networking/poll_groups/PlayerPollGroup.hpp"
#include "communication/NetworkEvent.hpp"
#include "communication/NetworkEventDispatcher.hpp"
#include "networking/poll_groups/PollGroupBase.hpp"

#include "communication/NetworkMessage.hpp"

#include "spdlog/spdlog.h"

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
  const std::shared_ptr<NetworkEventDispatcher>& network_event_dispatcher)
{
    network_event_dispatcher_ = network_event_dispatcher;
}

void PlayerPollGroup::SetWorld(const std::shared_ptr<IWorld>& world)
{
    world_ = world;
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
            spdlog::error("Error checking for messages");
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
    unsigned int soldier_id = world_->CreateSoldier().id;
    spdlog::info("OnAssignPlayerId: {}", soldier_id);
    NetworkMessage network_message(NetworkEvent::AssignPlayerId, soldier_id);
    SendNetworkMessage(connection.connection_handle, network_message);

    auto spawn_position = world_->SpawnSoldier(soldier_id);
    SendNetworkMessage(
      connection.connection_handle,
      { NetworkEvent::SpawnSoldier, soldier_id, spawn_position.x, spawn_position.y });
}
} // namespace Soldat
