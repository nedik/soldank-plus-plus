#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include "core/World.hpp"

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

    std::unique_ptr<World> world_;
};
} // namespace Soldat

#endif
