#include "networking/interface/NetworkingInterface.hpp"

#include "spdlog/spdlog.h"

#include <vector>
#include <functional>
#include <array>

namespace Soldat::NetworkingInterface
{
namespace
{
ISteamNetworkingSockets* interface;
std::vector<std::function<void(SteamNetConnectionStatusChangedCallback_t*)>> observers;

void SteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* p_info)
{
    for (const auto& observer : observers) {
        observer(p_info);
    }
}
} // namespace

void Init()
{
    interface = SteamNetworkingSockets();
}

std::shared_ptr<Connection> CreateConnection(const char* server_ip, std::uint16_t server_port)
{
    SteamNetworkingIPAddr server_addr{};
    server_addr.Clear();
    server_addr.ParseString(server_ip); // TODO: ip=localhost doesn't work
    server_addr.m_port = server_port;
    spdlog::info("Connecting to server");

    SteamNetworkingConfigValue_t opt{};
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
               (void*)SteamNetConnectionStatusChangedCallback);
    HSteamNetConnection connection_handle = interface->ConnectByIPAddress(server_addr, 1, &opt);
    if (connection_handle == k_HSteamNetConnection_Invalid) {
        spdlog::error("Failed to create connection");
    }

    return std::make_shared<Connection>(interface, connection_handle);
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
    observers.clear();
}
} // namespace Soldat::NetworkingInterface
