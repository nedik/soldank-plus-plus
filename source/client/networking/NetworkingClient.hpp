#ifndef __NETWORKING_CLIENT_HPP__
#define __NETWORKING_CLIENT_HPP__

#include "networking/Connection.hpp"

#include "communication/NetworkEventDispatcher.hpp"

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#include <memory>

namespace Soldat
{
class NetworkingClient
{
public:
    NetworkingClient();

    void Update(const std::shared_ptr<NetworkEventDispatcher>& network_event_dispatcher);

private:
    void OnSteamNetConnectionStatusChanged(
      SteamNetConnectionStatusChangedCallback_t* connection_info);

    std::shared_ptr<Connection> connection_;
    std::shared_ptr<NetworkEventDispatcher> network_event_dispatcher_;
};
} // namespace Soldat

#endif
