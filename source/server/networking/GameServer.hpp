#ifndef __GAME_SERVER_HPP__
#define __GAME_SERVER_HPP__

#include "networking/poll_groups/EntryPollGroup.hpp"
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
    ~GameServer();

    GameServer(GameServer&& other) = delete;
    GameServer& operator=(GameServer&& other) = delete;
    GameServer(GameServer& other) = delete;
    GameServer& operator=(GameServer& other) = delete;

    void Update();

private:
    std::unique_ptr<EntryPollGroup> entry_poll_group_;
    std::shared_ptr<PlayerPollGroup> player_poll_group_;

    void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* p_info);
};
} // namespace Soldat

#endif