#include "application/Application.hpp"

#include "communication/NetworkPackets.hpp"
#include "networking/events/ServerNetworkEventObserver.hpp"

#include "spdlog/spdlog.h"

#include <steam/steamnetworkingsockets.h>

#include <span>
#include <cstdlib>
#include <format>

namespace Soldat
{
SteamNetworkingMicroseconds Application::log_time_zero_ = 0;

void Application::DebugOutput(ESteamNetworkingSocketsDebugOutputType output_type,
                              const char* message)
{
    SteamNetworkingMicroseconds time = SteamNetworkingUtils()->GetLocalTimestamp() - log_time_zero_;
    spdlog::info("{} {}", (double)time * 1e-6, message);
    fflush(stdout);
    if (output_type == k_ESteamNetworkingSocketsDebugOutputType_Bug) {
        exit(1);
    }
}

Application::Application()
    : world_(std::make_shared<World>())
{
    SteamDatagramErrMsg err_msg;
    if (!GameNetworkingSockets_Init(nullptr, err_msg)) {
        spdlog::error("GameNetworkingSockets_Init failed. {}", std::span(err_msg).data());
    }

    log_time_zero_ = SteamNetworkingUtils()->GetLocalTimestamp();

    SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Msg,
                                                   DebugOutput);

    server_network_event_observer_ = std::make_shared<ServerNetworkEventObserver>(world_);
    server_network_event_dispatcher_ =
      std::make_shared<NetworkEventDispatcher>(server_network_event_observer_);
    game_server_ = std::make_shared<GameServer>(server_network_event_dispatcher_, world_);
}

Application::~Application()
{
    GameNetworkingSockets_Kill();
}

void Application::Run()
{
    spdlog::info("Server started!");

    world_->SetShouldStopGameLoopCallback([&]() { return false; });
    world_->SetPreGameLoopIterationCallback([&]() {});
    world_->SetPreWorldUpdateCallback([&]() {});
    world_->SetPostWorldUpdateCallback([&](const std::shared_ptr<State>& state) {
        for (const auto& soldier : state->soldiers) {
            UpdateSoldierStatePacket update_soldier_state_packet{ .game_tick = state->game_tick,
                                                                  .id = soldier.id,
                                                                  .position_x =
                                                                    soldier.particle.position.x,
                                                                  .position_y =
                                                                    soldier.particle.position.y };
            game_server_->SendNetworkMessageToAll(
              { NetworkEvent::UpdateSoldierState, update_soldier_state_packet });
        }
    });
    world_->SetPostGameLoopIterationCallback([&](const std::shared_ptr<State>& state,
                                                 double frame_percent,
                                                 int last_fps) { game_server_->Update(); });

    world_->RunLoop(60);

    // Give connections time to finish up.  This is an application layer protocol
    // here, it's not TCP.  Note that if you have an application and you need to be
    // more sure about cleanup, you won't be able to do this.  You will need to send
    // a message and then either wait for the peer to close the connection, or
    // you can pool the connection to see if any reliable data is pending.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
} // namespace Soldat
