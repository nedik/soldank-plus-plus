#ifndef __INETWORKING_CLIENT_HPP__
#define __INETWORKING_CLIENT_HPP__

#include "communication/NetworkEventDispatcher.hpp"

#include <memory>

namespace Soldat
{
class INetworkingClient
{
public:
    virtual ~INetworkingClient() = default;

    virtual void Update(
      const std::shared_ptr<NetworkEventDispatcher>& network_event_dispatcher) = 0;
};
} // namespace Soldat

#endif
