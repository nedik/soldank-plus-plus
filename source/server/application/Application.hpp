#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include "core/World.hpp"

#include "networking/GameServer.hpp"
#include "networking/events/ServerNetworkEventDispatcher.hpp"
#include "networking/events/ServerNetworkEventObserver.hpp"

#include <steam/isteamnetworkingutils.h>

#include <memory>

namespace Soldat
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

    std::shared_ptr<GameServer> game_server_;
    std::shared_ptr<IWorld> world_;
    std::shared_ptr<ServerNetworkEventObserver> server_network_event_observer_;
    std::shared_ptr<ServerNetworkEventDispatcher> server_network_event_dispatcher_;
};
} // namespace Soldat

#endif
