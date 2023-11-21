#ifndef __NETWORKING_INTERFACE_HPP__
#define __NETWORKING_INTERFACE_HPP__

#include "networking/Connection.hpp"

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#include <functional>
#include <memory>

namespace Soldat::NetworkingInterface
{
void Init();
std::shared_ptr<Connection> CreateConnection();
void PollConnectionStateChanges();
void RegisterObserver(
  const std::function<void(SteamNetConnectionStatusChangedCallback_t*)>& observer);
void Free();
} // namespace Soldat::NetworkingInterface

#endif
