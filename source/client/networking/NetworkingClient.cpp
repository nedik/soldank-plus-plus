#include "networking/NetworkingClient.hpp"

#include "networking/interface/NetworkingInterface.hpp"

#include "spdlog/spdlog.h"

#include <string>
#include <span>
#include <utility>

namespace Soldat
{
NetworkingClient::NetworkingClient(const char* server_ip, std::uint16_t server_port)
{
    NetworkingInterface::Init();
    NetworkingInterface::RegisterObserver(
      [this](SteamNetConnectionStatusChangedCallback_t* connection_info) {
          OnSteamNetConnectionStatusChanged(connection_info);
      });

    connection_ = NetworkingInterface::CreateConnection(server_ip, server_port);
}

void NetworkingClient::Update(
  const std::shared_ptr<NetworkEventDispatcher>& network_event_dispatcher)
{
    connection_->PollIncomingMessages(network_event_dispatcher);
    NetworkingInterface::PollConnectionStateChanges();
}

void NetworkingClient::SendNetworkMessage(const NetworkMessage& network_message)
{
    connection_->SendNetworkMessage(network_message);
}

void NetworkingClient::OnSteamNetConnectionStatusChanged(
  SteamNetConnectionStatusChangedCallback_t* connection_info)
{
    connection_->AssertConnectionInfo(connection_info);

    switch (connection_info->m_info.m_eState) {
        case k_ESteamNetworkingConnectionState_None:
            // NOTE: We will get callbacks here when we destroy connections.  You can ignore these.
            break;

        case k_ESteamNetworkingConnectionState_ClosedByPeer:
        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: {
            // Print an appropriate message
            if (connection_info->m_eOldState == k_ESteamNetworkingConnectionState_Connecting) {
                // Note: we could distinguish between a timeout, a rejected connection,
                // or some other transport problem.
                spdlog::info("We sought the remote host, yet our efforts were met with defeat. {}",
                             std::span<char>(connection_info->m_info.m_szEndDebug).data());
            } else if (connection_info->m_info.m_eState ==
                       k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
                spdlog::info("Alas, troubles beset us; we have lost contact with the host. {}",
                             std::span<char>(connection_info->m_info.m_szEndDebug).data());
            } else {
                // NOTE: We could check the reason code for a normal disconnection
                spdlog::info("The host hath bidden us farewell. {}",
                             std::span<char>(connection_info->m_info.m_szEndDebug).data());
            }

            connection_->CloseConnection();
            // connection_handle_ = k_HSteamNetConnection_Invalid;
            break;
        }

        case k_ESteamNetworkingConnectionState_Connecting:
            // We will get this callback when we start connecting.
            // We can ignore this.
            break;

        case k_ESteamNetworkingConnectionState_Connected:
            spdlog::info("Connected to server OK");
            break;

        default:
            // Silences -Wswitch
            break;
    }
}
} // namespace Soldat
