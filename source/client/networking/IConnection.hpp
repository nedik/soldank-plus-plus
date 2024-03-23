#ifndef __ICONNECTION_HPP__
#define __ICONNECTION_HPP__

#include "communication/NetworkEventDispatcher.hpp"

#include <steam/isteamnetworkingutils.h>

namespace Soldat
{
class IConnection
{
public:
    virtual ~IConnection() = default;

    virtual void PollIncomingMessages(
      const std::shared_ptr<NetworkEventDispatcher>& network_event_dispatcher) = 0;

    virtual void CloseConnection() = 0;

    virtual void AssertConnectionInfo(
      SteamNetConnectionStatusChangedCallback_t* connection_info) const = 0;

    virtual void SendNetworkMessage(const NetworkMessage& network_message) = 0;
};
} // namespace Soldat

#endif
