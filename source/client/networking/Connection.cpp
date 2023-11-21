#include "networking/Connection.hpp"

#include <iostream>
#include <string>
#include <cassert>

namespace Soldat
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

void Connection::PollIncomingMessages()
{
    ISteamNetworkingMessage* p_incoming_msg = nullptr;
    int num_msgs = interface_->ReceiveMessagesOnConnection(connection_handle_, &p_incoming_msg, 1);
    if (num_msgs == 0) {
        return;
    }
    if (num_msgs < 0) {
        std::cout << "Error checking for messages" << std::endl;
    }

    std::string message_from_server;
    message_from_server.assign(static_cast<char*>(p_incoming_msg->m_pData),
                               p_incoming_msg->m_cbSize);

    std::cout << message_from_server << std::endl;

    // We don't need this anymore.
    p_incoming_msg->Release();
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
} // namespace Soldat
