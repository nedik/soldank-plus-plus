#include "Application.hpp"

#include "application/ClientNetworkEventObserver.hpp"
#include "application/config/Config.hpp"
#include "application/input/Keyboard.hpp"
#include "application/input/Mouse.hpp"

#include "networking/NetworkingClient.hpp"

#include "core/World.hpp"

#include "rendering/Scene.hpp"
#include "rendering/ClientState.hpp"

#include "communication/NetworkEventDispatcher.hpp"

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <format>
#include <span>

namespace Soldat::Application
{
std::unique_ptr<Window> window;
std::shared_ptr<World> world;
std::unique_ptr<NetworkingClient> networking_client;
std::shared_ptr<ClientState> client_state;
std::shared_ptr<NetworkEventDispatcher> client_network_event_dispatcher;
std::shared_ptr<INetworkEventObserver> client_network_event_observer;

SteamNetworkingMicroseconds log_time_zero;

void DebugOutput(ESteamNetworkingSocketsDebugOutputType output_type, const char* message)
{
    SteamNetworkingMicroseconds time = SteamNetworkingUtils()->GetLocalTimestamp() - log_time_zero;
    std::cout << std::format("{} {}", (double)time * 1e-6, message) << std::endl;
    fflush(stdout);
    if (output_type == k_ESteamNetworkingSocketsDebugOutputType_Bug) {
        exit(1);
    }
}

void Init()
{
    window = std::make_unique<Window>();
    world = std::make_shared<World>();
    client_state = std::make_shared<ClientState>();
    client_network_event_observer =
      std::make_shared<ClientNetworkEventObserver>(world, client_state);
    client_network_event_dispatcher =
      std::make_shared<NetworkEventDispatcher>(client_network_event_observer);

    SteamDatagramErrMsg err_msg;
    if (!GameNetworkingSockets_Init(nullptr, err_msg)) {
        std::cout << "GameNetworkingSockets_Init failed. " << std::span(err_msg).data()
                  << std::endl;
    }

    log_time_zero = SteamNetworkingUtils()->GetLocalTimestamp();

    SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Msg,
                                                   DebugOutput);

    networking_client = std::make_unique<NetworkingClient>();
}

void UpdateMouseButton(int button, int action)
{
    if (client_state->client_soldier_id.has_value()) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            world->UpdateFireButtonState(*client_state->client_soldier_id, action == GLFW_PRESS);
        }

        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            world->UpdateJetsButtonState(*client_state->client_soldier_id, action == GLFW_PRESS);
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
    world->SetPreWorldUpdateCallback([&]() {
        glm::vec2 mouse_position = { Mouse::GetX(), Mouse::GetY() };

        client_state->game_width = 640.0;
        client_state->game_height = 480.0;
        client_state->camera_prev = client_state->camera;

        client_state->mouse.x = mouse_position.x;
        client_state->mouse.y = mouse_position.y;

        if (client_state->client_soldier_id.has_value()) {
            unsigned int client_soldier_id = *client_state->client_soldier_id;

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
            client_state->camera = world->GetSoldier(client_soldier_id).camera;
        } else {
            client_state->camera = { 0.0F, 0.0F };
        }
    });
    world->SetPostGameLoopIterationCallback(
      [&](const std::shared_ptr<State>& state, double frame_percent, int last_fps) {
          scene.Render(state, *client_state, frame_percent, last_fps);

          window->SwapBuffers();
          window->PollInput();

          networking_client->Update(client_network_event_dispatcher);
      });

    world->RunLoop(Config::FPS_LIMIT);
}

void Free()
{
    window.reset(nullptr);
    networking_client.reset(nullptr);

    // Give connections time to finish up.  This is an application layer protocol
    // here, it's not TCP.  Note that if you have an application and you need to be
    // more sure about cleanup, you won't be able to do this.  You will need to send
    // a message and then either wait for the peer to close the connection, or
    // you can pool the connection to see if any reliable data is pending.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    GameNetworkingSockets_Kill();
}
} // namespace Soldat::Application
