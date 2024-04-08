#include "networking/poll_groups/PlayerPollGroup.hpp"
#include "communication/NetworkEvent.hpp"
#include "communication/NetworkEventDispatcher.hpp"
#include "networking/poll_groups/PollGroupBase.hpp"

#include "communication/NetworkMessage.hpp"

#include "spdlog/spdlog.h"

#include <cassert>
#include <span>

namespace Soldank
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
        NetworkMessage network_message(received_bytes);

        unsigned int connection_id = incoming_message->m_conn;

        ConnectionMetadata connection_metadata{ .connection_id = connection_id,
                                                .send_message_to_connection =
                                                  [&](const NetworkMessage& message) {
                                                      SendNetworkMessage(connection_id, message);
                                                  } };

        incoming_message->Release();
        network_event_dispatcher_->ProcessNetworkMessage(connection_metadata, network_message);
    }
}

void PlayerPollGroup::AcceptConnection(
  SteamNetConnectionStatusChangedCallback_t* /*new_connection_info*/)
{
}

void PlayerPollGroup::OnAssignConnection(Connection& connection)
{
    const auto& state = world_->GetStateManager()->GetState();
    for (const auto& soldier : state.soldiers) {
        NetworkMessage network_message(NetworkEvent::SoldierInfo, soldier.id);
        SendReliableNetworkMessage(connection.connection_handle, network_message);
    }

    unsigned int soldier_id = world_->CreateSoldier().id;
    connection.soldier_id = soldier_id;
    spdlog::info("OnAssignPlayerId: {}", soldier_id);
    NetworkMessage network_message(NetworkEvent::AssignPlayerId, soldier_id);
    SendNetworkMessage(connection.connection_handle, network_message);

    auto spawn_position = world_->SpawnSoldier(soldier_id);
    SendNetworkMessage(
      connection.connection_handle,
      { NetworkEvent::SpawnSoldier, soldier_id, spawn_position.x, spawn_position.y });

    network_message = { NetworkEvent::SoldierInfo, soldier_id };
    SendReliableNetworkMessage(connection.connection_handle, network_message);

    SendReliableNetworkMessageToAll(network_message, connection.connection_handle);
}
} // namespace Soldank
