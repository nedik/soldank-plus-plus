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

    server_state_ = std::make_shared<ServerState>();
    server_state_->last_processed_input_id = 0;
    server_network_event_observer_ =
      std::make_shared<ServerNetworkEventObserver>(world_, server_state_);
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
    world_->SetPreWorldUpdateCallback([&]() { game_server_->Update(); });
    world_->SetPostWorldUpdateCallback([&](const std::shared_ptr<State>& state) {
        for (const auto& soldier : state->soldiers) {
            SoldierStatePacket update_soldier_state_packet{
                .game_tick = state->game_tick,
                .player_id = soldier.id,
                .position_x = soldier.particle.position.x,
                .position_y = soldier.particle.position.y,
                .old_position_x = soldier.particle.old_position.x,
                .old_position_y = soldier.particle.old_position.y,
                .body_animation_type = soldier.body_animation.GetType(),
                .body_animation_frame = soldier.body_animation.GetFrame(),
                .body_animation_speed = soldier.body_animation.GetSpeed(),
                .legs_animation_type = soldier.legs_animation.GetType(),
                .legs_animation_frame = soldier.legs_animation.GetFrame(),
                .legs_animation_speed = soldier.legs_animation.GetSpeed(),
                .velocity_x = soldier.particle.GetVelocity().x,
                .velocity_y = soldier.particle.GetVelocity().y,
                .force_x = soldier.particle.GetForce().x,
                .force_y = soldier.particle.GetForce().y,
                .on_ground = soldier.on_ground,
                .on_ground_for_law = soldier.on_ground_for_law,
                .on_ground_last_frame = soldier.on_ground_last_frame,
                .on_ground_permanent = soldier.on_ground_permanent,
                .last_processed_input_id = server_state_->last_processed_input_id
            };
            game_server_->SendNetworkMessageToAll(
              { NetworkEvent::SoldierState, update_soldier_state_packet });
        }

        // for (const auto& soldier : state->soldiers) {
        //     if (soldier.active) {
        //         spdlog::info("{}, Player {} pos: {}, {}; velocity: {} {}; force: {} {}",
        //                      server_state_->last_processed_input_id,
        //                      soldier.id,
        //                      soldier.particle.position.x,
        //                      soldier.particle.position.y,
        //                      soldier.particle.GetVelocity().x,
        //                      soldier.particle.GetVelocity().y,
        //                      soldier.particle.GetForce().x,
        //                      soldier.particle.GetForce().y);
        //     }
        // }
    });
    world_->SetPostGameLoopIterationCallback(
      [&](const std::shared_ptr<State>& state, double frame_percent, int last_fps) {});

    world_->RunLoop(60);

    // Give connections time to finish up.  This is an application layer protocol
    // here, it's not TCP.  Note that if you have an application and you need to be
    // more sure about cleanup, you won't be able to do this.  You will need to send
    // a message and then either wait for the peer to close the connection, or
    // you can pool the connection to see if any reliable data is pending.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
} // namespace Soldat
