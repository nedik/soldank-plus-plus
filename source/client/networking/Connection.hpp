#ifndef __CONNECTION_HPP__
#define __CONNECTION_HPP__

#include "communication/NetworkEventDispatcher.hpp"

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#include <memory>

namespace Soldat
{
class Connection
{
public:
    Connection(ISteamNetworkingSockets* interface, HSteamNetConnection connection_handle);

    void PollIncomingMessages(
      const std::shared_ptr<NetworkEventDispatcher>& network_event_dispatcher);

    void CloseConnection();

    void AssertConnectionInfo(SteamNetConnectionStatusChangedCallback_t* connection_info) const;

private:
    ISteamNetworkingSockets* interface_;
    HSteamNetConnection connection_handle_;
};
} // namespace Soldat

#endif
