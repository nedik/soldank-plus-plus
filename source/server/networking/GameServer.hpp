#ifndef __GAME_SERVER_HPP__
#define __GAME_SERVER_HPP__

#include "networking/poll_groups/PlayerPollGroup.hpp"

#include <steam/steamnetworkingsockets.h>

#include <unordered_map>
#include <string>
#include <memory>

namespace Soldat
{
class GameServer
{
public:
    GameServer();

    void Run();

private:
    std::unique_ptr<PlayerPollGroup> player_poll_group_;

    void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* p_info);
};
} // namespace Soldat

#endif
