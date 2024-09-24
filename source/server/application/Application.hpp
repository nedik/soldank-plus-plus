#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include "application/ServerState.hpp"

#include "core/World.hpp"

#include "networking/GameServer.hpp"
#include "networking/LobbyClient.hpp"

#include "scripting/IScriptingEngine.hpp"

#include <steam/isteamnetworkingutils.h>

#include <memory>

namespace Soldank
{
class Application
{
public:
    Application();
    ~Application();

    Application(Application&& other) = delete;
    Application& operator=(Application&& other) = delete;
    Application(Application& other) = delete;
    Application& operator=(Application& other) = delete;

    void Run();

private:
    static void DebugOutput(ESteamNetworkingSocketsDebugOutputType output_type,
                            const char* message);

    static SteamNetworkingMicroseconds log_time_zero_;

    std::shared_ptr<IGameServer> game_server_;
    std::shared_ptr<IWorld> world_;
    std::shared_ptr<NetworkEventDispatcher> server_network_event_dispatcher_;
    std::shared_ptr<ServerState> server_state_;
    std::shared_ptr<IScriptingEngine> scripting_engine_;
    std::shared_ptr<LobbyClient> lobby_client_;
};
} // namespace Soldank

#endif
