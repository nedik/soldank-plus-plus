#include "networking/interface/NetworkingInterface.hpp"

#include <steam/steamnetworkingsockets.h>

#include "spdlog/spdlog.h"

namespace Soldank::NetworkingInterface
{
namespace
{
ISteamNetworkingSockets* interface;
HSteamListenSocket listen_socket_handle;
std::vector<std::function<void(SteamNetConnectionStatusChangedCallback_t*)>> observers;

void SteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* p_info)
{
    for (const auto& observer : observers) {
        observer(p_info);
    }
}
} // namespace

namespace Hidden
{
ISteamNetworkingSockets* GetInterface()
{
    return interface;
}
} // namespace Hidden

void Init(std::uint16_t port)
{
    interface = SteamNetworkingSockets();
    SteamNetworkingIPAddr server_local_addr{};
    server_local_addr.Clear();
    server_local_addr.m_port = port;
    SteamNetworkingConfigValue_t opt{};
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
               (void*)SteamNetConnectionStatusChangedCallback);
    listen_socket_handle = interface->CreateListenSocketIP(server_local_addr, 1, &opt);
    if (listen_socket_handle == k_HSteamListenSocket_Invalid) {
        spdlog::error("Failed to listen on port {}", port);
    }
}

void PollConnectionStateChanges()
{
    interface->RunCallbacks();
}

void RegisterObserver(
  const std::function<void(SteamNetConnectionStatusChangedCallback_t*)>& observer)
{
    observers.push_back(observer);
}

void Free()
{
    interface->CloseListenSocket(listen_socket_handle);
    listen_socket_handle = k_HSteamListenSocket_Invalid;
    observers.clear();
}
} // namespace Soldank::NetworkingInterface
