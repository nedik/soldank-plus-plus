#include "networking/LagConnection.hpp"

#include "communication/NetworkEventDispatcher.hpp"
#include "communication/NetworkMessage.hpp"

#include "spdlog/spdlog.h"

#include <string>
#include <cassert>
#include <span>

namespace Soldat
{
LagConnection::LagConnection(ISteamNetworkingSockets* interface,
                             HSteamNetConnection connection_handle)
    : interface_(interface)
    , connection_handle_(connection_handle)
    , lag_to_add_(0)
{
    std::string message = "test name";
    interface_->SendMessageToConnection(connection_handle_,
                                        message.c_str(),
                                        (uint32)message.length(),
                                        k_nSteamNetworkingSend_Reliable,
                                        nullptr);
}

void LagConnection::PollIncomingMessages(
  const std::shared_ptr<NetworkEventDispatcher>& network_event_dispatcher)
{
    for (auto it = messages_to_send_.begin(); it != messages_to_send_.end();) {
        if (it->first <= std::chrono::system_clock::now()) {
            interface_->SendMessageToConnection(connection_handle_,
                                                it->second.GetData().data(),
                                                it->second.GetData().size(),
                                                k_nSteamNetworkingSend_Unreliable,
                                                nullptr);
            it = messages_to_send_.erase(it);
        } else {
            it++;
        }
    }

    while (true) {
        ISteamNetworkingMessage* p_incoming_msg = nullptr;
        int num_msgs =
          interface_->ReceiveMessagesOnConnection(connection_handle_, &p_incoming_msg, 1);
        if (num_msgs == 0) {
            for (auto it = messages_to_receive_.begin(); it != messages_to_receive_.end();) {
                if (it->when_to_receive <= std::chrono::system_clock::now()) {
                    network_event_dispatcher->ProcessNetworkMessage(it->connection_metadata,
                                                                    it->received_message);
                    it = messages_to_receive_.erase(it);
                } else {
                    it++;
                }
            }
            return;
        }
        if (num_msgs < 0) {
            spdlog::error("Error checking for messages");
            return;
        }

        std::span<const char> received_bytes{ static_cast<char*>(p_incoming_msg->m_pData),
                                              static_cast<unsigned int>(p_incoming_msg->m_cbSize) };

        NetworkMessage received_message(received_bytes);
        MessageToReceive message_to_receive{
            .connection_metadata = { .connection_id = p_incoming_msg->m_conn,
                                     .send_message_to_connection =
                                       [](const NetworkMessage& message) {} },
            .when_to_receive = std::chrono::system_clock::now() + lag_to_add_,
            .received_message = received_message,
        };
        messages_to_receive_.push_back(message_to_receive);
        p_incoming_msg->Release();
    }
}

void LagConnection::CloseConnection()
{
    interface_->CloseConnection(connection_handle_, 0, nullptr, false);
}

void LagConnection::AssertConnectionInfo(
  SteamNetConnectionStatusChangedCallback_t* connection_info) const
{
    assert(connection_info->m_hConn == connection_handle_ ||
           connection_handle_ == k_HSteamNetConnection_Invalid);
}

void LagConnection::SendNetworkMessage(const NetworkMessage& network_message)
{
    messages_to_send_.emplace_back(std::chrono::system_clock::now() + lag_to_add_, network_message);
}

void LagConnection::SetLag(unsigned int milliseconds)
{
    lag_to_add_ = std::chrono::duration<unsigned int, std::milli>(milliseconds);
}
} // namespace Soldat
