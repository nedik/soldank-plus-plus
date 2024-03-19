#include "Application.hpp"

#include "application/ClientNetworkEventObserver.hpp"
#include "application/config/Config.hpp"
#include "application/input/Keyboard.hpp"
#include "application/input/Mouse.hpp"

#include "communication/NetworkPackets.hpp"
#include "networking/NetworkingClient.hpp"

#include "core/World.hpp"

#include "rendering/Scene.hpp"
#include "rendering/ClientState.hpp"

#include "communication/NetworkEventDispatcher.hpp"

#include "spdlog/spdlog.h"

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#include <GLFW/glfw3.h>

#include <SimpleIni.h>

#include <cmath>
#include <memory>
#include <chrono>
#include <thread>
#include <span>

namespace Soldat::Application
{
std::unique_ptr<Window> window;
std::shared_ptr<IWorld> world;
std::unique_ptr<INetworkingClient> networking_client;
std::shared_ptr<ClientState> client_state;
std::shared_ptr<NetworkEventDispatcher> client_network_event_dispatcher;
std::shared_ptr<INetworkEventObserver> client_network_event_observer;

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

    if (is_online) {
        spdlog::info("Connecting to {}:{}", server_ip, server_port);
        client_network_event_observer =
          std::make_shared<ClientNetworkEventObserver>(world, client_state);
        client_network_event_dispatcher =
          std::make_shared<NetworkEventDispatcher>(client_network_event_observer);

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

void UpdateMouseButton(int button, int action)
{
    if (client_state->client_soldier_id.has_value()) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            world->UpdateFireButtonState(*client_state->client_soldier_id, action == GLFW_PRESS);
        }
    }
}

void Run()
{
    window->Create();

    Scene scene(world->GetState());
    Mouse::SubscribeButtonObserver(
      [](int button, int action) { UpdateMouseButton(button, action); });

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
            networking_client->Update(client_network_event_dispatcher);

            auto current_time = std::chrono::system_clock::now();

            if (client_state->last_ping_check_time.has_value()) {
                std::chrono::duration<double> diff =
                  current_time - *client_state->last_ping_check_time;

                if (diff.count() > 9999.0) {
                    client_state->last_ping_check_time = std::nullopt;
                    client_state->last_ping = 9999;
                }
            }

            if ((world->GetState()->game_tick % 60 == 0) &&
                !client_state->last_ping_check_time.has_value()) {

                client_state->last_ping_check_time = current_time;
                networking_client->SendNetworkMessage({ NetworkEvent::PingCheck });
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
            for (const auto& soldier : world->GetState()->soldiers) {
                if (soldier.id == client_soldier_id && soldier.active) {
                    is_soldier_active = true;
                }
            }

            if (is_soldier_active) {
                world->UpdateLeftButtonState(client_soldier_id, Keyboard::Key(GLFW_KEY_A));
                world->UpdateRightButtonState(client_soldier_id, Keyboard::Key(GLFW_KEY_D));
                world->UpdateJumpButtonState(client_soldier_id, Keyboard::Key(GLFW_KEY_W));
                world->UpdateRightButtonState(client_soldier_id, Keyboard::Key(GLFW_KEY_D));
                world->UpdateCrouchButtonState(client_soldier_id, Keyboard::Key(GLFW_KEY_S));
                world->UpdateChangeButtonState(client_soldier_id, Keyboard::Key(GLFW_KEY_Q));
                world->UpdateThrowGrenadeButtonState(client_soldier_id, Keyboard::Key(GLFW_KEY_E));
                world->UpdateDropButtonState(client_soldier_id, Keyboard::Key(GLFW_KEY_F));
                world->UpdateProneButtonState(client_soldier_id, Keyboard::Key(GLFW_KEY_X));

                world->UpdateMousePosition(client_soldier_id, mouse_position);
                world->UpdateJetsButtonState(client_soldier_id,
                                             Mouse::Button(GLFW_MOUSE_BUTTON_RIGHT));
                client_state->camera = world->GetSoldier(client_soldier_id).camera;
            } else {
                client_state->camera = { 0.0F, 0.0F };
            }

            if (is_online) {
                SoldierInputPacket update_soldier_state_packet{
                    .input_sequence_id = input_sequence_id,
                    .game_tick = world->GetState()->game_tick,
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
            }
        } else {
            client_state->camera = { 0.0F, 0.0F };
        }
    });
    world->SetPostWorldUpdateCallback([&](const std::shared_ptr<State>& state) {
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
    world->SetPostGameLoopIterationCallback(
      [&](const std::shared_ptr<State>& state, double frame_percent, int last_fps) {
          if (!client_state->objects_interpolation) {
              frame_percent = 1.0F;
          }
          scene.Render(state, *client_state, frame_percent, last_fps);

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
} // namespace Soldat::Application
