#ifndef __CONNECTION_HPP__
#define __CONNECTION_HPP__

#include "networking/IConnection.hpp"

#include "communication/NetworkEventDispatcher.hpp"

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#include <memory>

namespace Soldank
{
class Connection : public IConnection
{
public:
    Connection(ISteamNetworkingSockets* interface, HSteamNetConnection connection_handle);

    void PollIncomingMessages(
      const std::shared_ptr<NetworkEventDispatcher>& network_event_dispatcher) override;

    void CloseConnection() override;

    void AssertConnectionInfo(
      SteamNetConnectionStatusChangedCallback_t* connection_info) const override;

    void SendNetworkMessage(const NetworkMessage& network_message) override;

private:
    ISteamNetworkingSockets* interface_;
    HSteamNetConnection connection_handle_;
};
} // namespace Soldank

#endif
