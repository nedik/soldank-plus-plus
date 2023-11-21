#ifndef __NETWORKING_CLIENT_HPP__
#define __NETWORKING_CLIENT_HPP__

#include "networking/Connection.hpp"

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#include <memory>

namespace Soldat
{
class NetworkingClient
{
public:
    NetworkingClient();

    void Update();

private:
    void OnSteamNetConnectionStatusChanged(
      SteamNetConnectionStatusChangedCallback_t* connection_info);

    std::shared_ptr<Connection> connection_;
};
} // namespace Soldat

#endif
