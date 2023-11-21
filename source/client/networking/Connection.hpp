#ifndef __CONNECTION_HPP__
#define __CONNECTION_HPP__

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

namespace Soldat
{
class Connection
{
public:
    Connection(ISteamNetworkingSockets* interface, HSteamNetConnection connection_handle);

    void PollIncomingMessages();

    void CloseConnection();

    void AssertConnectionInfo(SteamNetConnectionStatusChangedCallback_t* connection_info) const;

private:
    ISteamNetworkingSockets* interface_;
    HSteamNetConnection connection_handle_;
};
} // namespace Soldat

#endif
