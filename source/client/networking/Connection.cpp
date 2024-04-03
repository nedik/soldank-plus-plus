#include "networking/Connection.hpp"

#include "communication/NetworkEventDispatcher.hpp"
#include "communication/NetworkMessage.hpp"

#include "spdlog/spdlog.h"

#include <string>
#include <cassert>
#include <span>

namespace Soldank
{
Connection::Connection(ISteamNetworkingSockets* interface, HSteamNetConnection connection_handle)
    : interface_(interface)
    , connection_handle_(connection_handle)
{
    std::string message = "test name";
    interface_->SendMessageToConnection(connection_handle_,
                                        message.c_str(),
                                        (uint32)message.length(),
                                        k_nSteamNetworkingSend_Reliable,
                                        nullptr);
}

void Connection::PollIncomingMessages(
  const std::shared_ptr<NetworkEventDispatcher>& network_event_dispatcher)
{
    while (true) {
        ISteamNetworkingMessage* p_incoming_msg = nullptr;
        int num_msgs =
          interface_->ReceiveMessagesOnConnection(connection_handle_, &p_incoming_msg, 1);
        if (num_msgs == 0) {
            return;
        }
        if (num_msgs < 0) {
            spdlog::error("Error checking for messages");
            return;
        }

        std::span<const char> received_bytes{ static_cast<char*>(p_incoming_msg->m_pData),
                                              static_cast<unsigned int>(p_incoming_msg->m_cbSize) };

        NetworkMessage received_message(received_bytes);
        ConnectionMetadata connection_metadata{ .connection_id = p_incoming_msg->m_conn,
                                                .send_message_to_connection =
                                                  [](const NetworkMessage& message) {} };
        // We don't need this anymore.
        p_incoming_msg->Release();
        network_event_dispatcher->ProcessNetworkMessage(connection_metadata, received_message);
    }
}

void Connection::CloseConnection()
{
    interface_->CloseConnection(connection_handle_, 0, nullptr, false);
}

void Connection::AssertConnectionInfo(
  SteamNetConnectionStatusChangedCallback_t* connection_info) const
{
    assert(connection_info->m_hConn == connection_handle_ ||
           connection_handle_ == k_HSteamNetConnection_Invalid);
}

void Connection::SendNetworkMessage(const NetworkMessage& network_message)
{
    interface_->SendMessageToConnection(connection_handle_,
                                        network_message.GetData().data(),
                                        network_message.GetData().size(),
                                        k_nSteamNetworkingSend_Unreliable,
                                        nullptr);
}
} // namespace Soldank
