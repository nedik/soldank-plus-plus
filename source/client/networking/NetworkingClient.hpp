#ifndef __NETWORKING_CLIENT_HPP__
#define __NETWORKING_CLIENT_HPP__

#include "networking/INetworkingClient.hpp"

#include "networking/IConnection.hpp"
#include "networking/Connection.hpp"

#include "communication/NetworkEventDispatcher.hpp"

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#include <memory>
#include <cstdint>

namespace Soldank
{
class NetworkingClient : public INetworkingClient
{
public:
    NetworkingClient(const char* server_ip, std::uint16_t server_port);

    void Update(const std::shared_ptr<NetworkEventDispatcher>& network_event_dispatcher) override;

    void SendNetworkMessage(const NetworkMessage& network_message) override;

    void SetLag(int lag_to_add_milliseconds) override;

private:
    void OnSteamNetConnectionStatusChanged(
      SteamNetConnectionStatusChangedCallback_t* connection_info);

    std::shared_ptr<IConnection> connection_;
    std::shared_ptr<NetworkEventDispatcher> network_event_dispatcher_;
};
} // namespace Soldank

#endif
