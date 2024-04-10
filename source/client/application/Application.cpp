#include "Application.hpp"

#include "application/config/Config.hpp"
#include "application/input/Keyboard.hpp"
#include "application/input/Mouse.hpp"

#include "communication/NetworkPackets.hpp"
#include "networking/NetworkingClient.hpp"
#include "networking/event_handlers/AssignPlayerIdNetworkEventHandler.hpp"
#include "networking/event_handlers/PingCheckNetworkEventHandler.hpp"
#include "networking/event_handlers/PlayerLeaveNetworkEventHandler.hpp"
#include "networking/event_handlers/ProjectileSpawnNetworkEventHandler.hpp"
#include "networking/event_handlers/SoldierInfoNetworkEventHandler.hpp"
#include "networking/event_handlers/SoldierStateNetworkEventHandler.hpp"
#include "networking/event_handlers/SpawnSoldierNetworkEventHandler.hpp"
#include "networking/event_handlers/KillSoldierNetworkEventHandler.hpp"

#include "core/World.hpp"

#include "rendering/Scene.hpp"
#include "rendering/ClientState.hpp"

#include "communication/NetworkEventDispatcher.hpp"

#include "spdlog/spdlog.h"

#include <spdlog/common.h>
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#include <GLFW/glfw3.h>

#include <SimpleIni.h>

#include <cmath>
#include <memory>
#include <chrono>
#include <thread>
#include <span>

namespace Soldank::Application
{
std::unique_ptr<Window> window;
std::shared_ptr<IWorld> world;
std::unique_ptr<INetworkingClient> networking_client;
std::shared_ptr<ClientState> client_state;
std::shared_ptr<NetworkEventDispatcher> client_network_event_dispatcher;

SteamNetworkingMicroseconds log_time_zero;

bool is_online;

void DebugOutput(ESteamNetworkingSocketsDebugOutputType output_type, const char* message)
{
    SteamNetworkingMicroseconds time = SteamNetworkingUtils()->GetLocalTimestamp() - log_time_zero;
    spdlog::info("{} {}", (double)time * 1e-6, message);
    fflush(stdout);
    if (output_type == k_ESteamNetworkingSocketsDebugOutputType_Bug) {
        exit(1);
    }
}

void Init()
{
    spdlog::set_level(spdlog::level::debug);

    CSimpleIniA ini_config;
    SI_Error rc = ini_config.LoadFile("debug_config.ini");
    std::string server_ip;
    int server_port = 0;
    if (rc < 0) {
        spdlog::warn("Error: INI File could not be loaded: debug_config.ini");
        is_online = false;
    } else {
        is_online = ini_config.GetBoolValue("Network", "Online");
        if (is_online) {
            spdlog::info("Online = true");
            const auto* ip = ini_config.GetValue("Network", "Server_IP");
            int port = ini_config.GetLongValue("Network", "Server_Port");
            if (ip == nullptr || port == 0) {
                spdlog::warn("Server_IP or Server_Port not set, setting is_online to false");
                is_online = false;
            } else {
                server_ip = ip;
                server_port = port;
            }
        } else {
            spdlog::info("Online = false");
        }
    }

    window = std::make_unique<Window>();
    world = std::make_shared<World>();
    client_state = std::make_shared<ClientState>();
    client_state->server_reconciliation = true;
    client_state->client_side_prediction = true;
    client_state->objects_interpolation = true;
    client_state->draw_server_pov_client_pos = true;

    if (is_online) {
        spdlog::info("Connecting to {}:{}", server_ip, server_port);
        std::vector<std::shared_ptr<INetworkEventHandler>> network_event_handlers{
            std::make_shared<AssignPlayerIdNetworkEventHandler>(world, client_state),
            std::make_shared<PingCheckNetworkEventHandler>(client_state),
            std::make_shared<PlayerLeaveNetworkEventHandler>(world),
            std::make_shared<ProjectileSpawnNetworkEventHandler>(world),
            std::make_shared<SoldierInfoNetworkEventHandler>(world, client_state),
            std::make_shared<SoldierStateNetworkEventHandler>(world, client_state),
            std::make_shared<SpawnSoldierNetworkEventHandler>(world),
            std::make_shared<KillSoldierNetworkEventHandler>(world)
        };
        client_network_event_dispatcher =
          std::make_shared<NetworkEventDispatcher>(network_event_handlers);

        SteamDatagramErrMsg err_msg;
        if (!GameNetworkingSockets_Init(nullptr, err_msg)) {
            spdlog::error("GameNetworkingSockets_Init failed. {}", std::span(err_msg).data());
        }

        log_time_zero = SteamNetworkingUtils()->GetLocalTimestamp();

        SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Msg,
                                                       DebugOutput);

        networking_client = std::make_unique<NetworkingClient>(server_ip.c_str(), server_port);
    }
}

void Run()
{
    window->Create();

    Scene scene(world->GetStateManager());

    world->SetShouldStopGameLoopCallback([&]() { return window->ShouldClose(); });
    world->SetPreGameLoopIterationCallback([&]() {
        if (Keyboard::KeyWentDown(GLFW_KEY_ESCAPE)) {
            window->Close();
        }
    });
    unsigned int input_sequence_id = 1;
    world->SetPreWorldUpdateCallback([&]() {
        // spdlog::info("netowrking_client->Update");
        if (is_online) {
            networking_client->SetLag(client_state->network_lag);
            networking_client->Update(client_network_event_dispatcher);

            if ((world->GetStateManager()->GetState().game_tick % 60 == 0)) {
                if (client_state->ping_timer.IsRunning()) {
                    client_state->ping_timer.Update();
                    if (client_state->ping_timer.IsOverThreshold()) {
                        networking_client->SendNetworkMessage({ NetworkEvent::PingCheck });
                    }
                } else {
                    client_state->ping_timer.Start();
                    networking_client->SendNetworkMessage({ NetworkEvent::PingCheck });
                }
            }
        }

        glm::vec2 mouse_position = { Mouse::GetX(), Mouse::GetY() };

        client_state->game_width = 640.0;
        client_state->game_height = 480.0;
        client_state->camera_prev = client_state->camera;

        client_state->mouse.x = mouse_position.x;
        client_state->mouse.y = mouse_position.y;

        if (client_state->client_soldier_id.has_value()) {
            unsigned int client_soldier_id = *client_state->client_soldier_id;
            bool is_soldier_active = false;
            bool is_soldier_alive = false;
            for (const auto& soldier : world->GetStateManager()->GetState().soldiers) {
                if (soldier.id == client_soldier_id && soldier.active) {
                    is_soldier_active = true;
                    is_soldier_alive = !soldier.dead_meat;
                }
            }

            if (is_soldier_active) {
                if (is_soldier_alive) {
                    world->UpdateLeftButtonState(client_soldier_id, Keyboard::Key(GLFW_KEY_A));
                    world->UpdateRightButtonState(client_soldier_id, Keyboard::Key(GLFW_KEY_D));
                    world->UpdateJumpButtonState(client_soldier_id, Keyboard::Key(GLFW_KEY_W));
                    world->UpdateRightButtonState(client_soldier_id, Keyboard::Key(GLFW_KEY_D));
                    world->UpdateCrouchButtonState(client_soldier_id, Keyboard::Key(GLFW_KEY_S));
                    world->UpdateChangeButtonState(client_soldier_id, Keyboard::Key(GLFW_KEY_Q));
                    world->UpdateThrowGrenadeButtonState(client_soldier_id,
                                                         Keyboard::Key(GLFW_KEY_E));
                    world->UpdateDropButtonState(client_soldier_id, Keyboard::Key(GLFW_KEY_F));
                    world->UpdateProneButtonState(client_soldier_id, Keyboard::Key(GLFW_KEY_X));

                    world->UpdateJetsButtonState(client_soldier_id,
                                                 Mouse::Button(GLFW_MOUSE_BUTTON_RIGHT));
                    world->UpdateFireButtonState(client_soldier_id,
                                                 Mouse::Button(GLFW_MOUSE_BUTTON_LEFT));
                }
                world->UpdateMousePosition(client_soldier_id, mouse_position);
                world->UpdateWeaponChoices(client_soldier_id,
                                           client_state->primary_weapon_type_choice,
                                           client_state->secondary_weapon_type_choice);
                client_state->camera = world->GetSoldier(client_soldier_id).camera;
            } else {
                client_state->camera = { 0.0F, 0.0F };
            }

            if (is_online) {
                SoldierInputPacket update_soldier_state_packet{
                    .input_sequence_id = input_sequence_id,
                    .game_tick = world->GetStateManager()->GetState().game_tick,
                    .player_id = client_soldier_id,
                    .position_x = world->GetSoldier(client_soldier_id).particle.position.x,
                    .position_y = world->GetSoldier(client_soldier_id).particle.position.y,
                    .mouse_position_x = mouse_position.x,
                    .mouse_position_y = mouse_position.y,
                    .control = world->GetSoldier(client_soldier_id).control
                };
                input_sequence_id++;
                if (client_state->server_reconciliation) {
                    client_state->pending_inputs.push_back(update_soldier_state_packet);
                }
                // spdlog::info("networking_client->SendNetworkMessage");
                networking_client->SendNetworkMessage(
                  { NetworkEvent::SoldierInput, update_soldier_state_packet });

                if (client_state->kill_button_just_pressed) {
                    client_state->kill_button_just_pressed = false;
                    networking_client->SendNetworkMessage({ NetworkEvent::KillCommand });
                }
            } else {
                if (client_state->kill_button_just_pressed) {
                    client_state->kill_button_just_pressed = false;
                    world->KillSoldier(client_soldier_id);
                }
            }
        } else {
            client_state->camera = { 0.0F, 0.0F };
        }
    });
    world->SetPostWorldUpdateCallback([&](const State& state) {
        // spdlog::info("Post World Update Call");
        // for (const auto& soldier : state->soldiers) {
        //     spdlog::info(
        //       "{}, Player {} pos: {}, {}; old_pos: {}, {}; velocity: {}, {}; force: {}, {}",
        //       input_sequence_id - 1,
        //       soldier.id,
        //       soldier.particle.position.x,
        //       soldier.particle.position.y,
        //       soldier.particle.old_position.x,
        //       soldier.particle.old_position.y,
        //       soldier.particle.GetVelocity().x,
        //       soldier.particle.GetVelocity().y,
        //       soldier.particle.GetForce().x,
        //       soldier.particle.GetForce().y);
        // }
    });
    world->SetPostGameLoopIterationCallback([&](const State& state,
                                                double frame_percent,
                                                int last_fps) {
        if (!client_state->objects_interpolation) {
            frame_percent = 1.0F;
        }
        scene.Render(world->GetStateManager()->GetState(), *client_state, frame_percent, last_fps);

        window->SwapBuffers();
        window->PollInput();
    });

    world->SetPreSoldierUpdateCallback([&](const Soldier& soldier) {
        if (!is_online) {
            return true;
        }

        if (client_state->client_soldier_id.has_value()) {
            if (*client_state->client_soldier_id == soldier.id &&
                client_state->client_side_prediction) {
                return true;
            }
        }

        return false;
    });
    world->SetPreProjectileSpawnCallback(
      [&](const BulletParams& bullet_params) { return !is_online; });

    if (!is_online) {
        const auto& soldier = world->CreateSoldier();
        client_state->client_soldier_id = soldier.id;
        world->SpawnSoldier(soldier.id);
    }

    world->RunLoop(Config::FPS_LIMIT);
}

void Free()
{
    window.reset(nullptr);
    networking_client.reset(nullptr);

    if (is_online) {
        // Give connections time to finish up.  This is an application layer protocol
        // here, it's not TCP.  Note that if you have an application and you need to be
        // more sure about cleanup, you won't be able to do this.  You will need to send
        // a message and then either wait for the peer to close the connection, or
        // you can pool the connection to see if any reliable data is pending.
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        GameNetworkingSockets_Kill();
    }
}
} // namespace Soldank::Application
