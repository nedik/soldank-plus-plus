#ifndef __NETWORKING_INTERFACE_HPP__
#define __NETWORKING_INTERFACE_HPP__

#include "networking/poll_groups/IPollGroup.hpp"

#include <functional>
#include <memory>
#include <concepts>
#include <type_traits>

struct SteamNetConnectionStatusChangedCallback_t;
class ISteamNetworkingSockets;

namespace Soldat::NetworkingInterface
{
namespace Hidden
{
ISteamNetworkingSockets* GetInterface();
}

void Init();
void PollConnectionStateChanges();
void RegisterObserver(
  const std::function<void(SteamNetConnectionStatusChangedCallback_t*)>& observer);

template<class TPollGroup = IPollGroup>
std::unique_ptr<TPollGroup> CreatePollGroup()
{
    return std::make_unique<TPollGroup>(Hidden::GetInterface());
}
void Free();
} // namespace Soldat::NetworkingInterface

#endif
