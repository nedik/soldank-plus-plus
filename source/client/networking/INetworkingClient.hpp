#ifndef __INETWORKING_CLIENT_HPP__
#define __INETWORKING_CLIENT_HPP__

#include "communication/NetworkEventDispatcher.hpp"

#include <memory>

namespace Soldank
{
class INetworkingClient
{
public:
    virtual ~INetworkingClient() = default;

    virtual void Update(
      const std::shared_ptr<NetworkEventDispatcher>& network_event_dispatcher) = 0;

    virtual void SendNetworkMessage(const NetworkMessage& network_message) = 0;

    virtual void SetLag(int lag_to_add_milliseconds) = 0;
};
} // namespace Soldank

#endif
