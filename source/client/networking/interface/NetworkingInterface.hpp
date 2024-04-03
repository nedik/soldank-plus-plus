#ifndef __NETWORKING_INTERFACE_HPP__
#define __NETWORKING_INTERFACE_HPP__

#include "networking/IConnection.hpp"
#include "networking/LagConnection.hpp"

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#include <functional>
#include <memory>
#include <cstdint>

namespace Soldank::NetworkingInterface
{
void Init();
std::shared_ptr<IConnection> CreateConnection(const char* server_ip, std::uint16_t server_port);
std::shared_ptr<LagConnection> CreateLagConnection(const char* server_ip,
                                                   std::uint16_t server_port);
void PollConnectionStateChanges();
void RegisterObserver(
  const std::function<void(SteamNetConnectionStatusChangedCallback_t*)>& observer);
void Free();
} // namespace Soldank::NetworkingInterface

#endif
