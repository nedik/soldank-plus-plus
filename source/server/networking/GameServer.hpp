#ifndef __GAME_SERVER_HPP__
#define __GAME_SERVER_HPP__

#include "networking/IGameServer.hpp"

#include "networking/poll_groups/EntryPollGroup.hpp"
#include "networking/poll_groups/PlayerPollGroup.hpp"

#include "communication/NetworkMessage.hpp"
#include "communication/NetworkEventDispatcher.hpp"

#include "core/IWorld.hpp"

#include <steam/steamnetworkingsockets.h>

#include <unordered_map>
#include <string>
#include <memory>

namespace Soldat
{
class GameServer : public IGameServer
{
public:
    GameServer(const std::shared_ptr<NetworkEventDispatcher>& network_event_dispatcher,
               const std::shared_ptr<IWorld>& world);
    ~GameServer() override;

    GameServer(GameServer&& other) = delete;
    GameServer& operator=(GameServer&& other) = delete;
    GameServer(GameServer& other) = delete;
    GameServer& operator=(GameServer& other) = delete;

    void Update() override;

    void SendNetworkMessage(unsigned int connection_id,
                            const NetworkMessage& network_message) override;
    void SendNetworkMessageToAll(const NetworkMessage& network_message) override;

private:
    std::unique_ptr<EntryPollGroup> entry_poll_group_;
    std::shared_ptr<PlayerPollGroup> player_poll_group_;

    void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* p_info);
};
} // namespace Soldat

#endif
