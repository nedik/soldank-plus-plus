#include "Application.hpp"

#include "application/config/Config.hpp"
#include "application/input/Keyboard.hpp"
#include "application/input/Mouse.hpp"

#include "networking/NetworkingClient.hpp"

#include "core/World.hpp"

#include "rendering/Scene.hpp"

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
std::unique_ptr<World> world;
std::unique_ptr<NetworkingClient> networking_client;
unsigned int player_soldier_id;

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
    world = std::make_unique<World>();

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
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        world->UpdateFireButtonState(player_soldier_id, action == GLFW_PRESS);
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        world->UpdateJetsButtonState(player_soldier_id, action == GLFW_PRESS);
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
        world->UpdateLeftButtonState(player_soldier_id, Keyboard::Key(GLFW_KEY_A));
        world->UpdateRightButtonState(player_soldier_id, Keyboard::Key(GLFW_KEY_D));
        world->UpdateJumpButtonState(player_soldier_id, Keyboard::Key(GLFW_KEY_W));
        world->UpdateRightButtonState(player_soldier_id, Keyboard::Key(GLFW_KEY_D));
        world->UpdateCrouchButtonState(player_soldier_id, Keyboard::Key(GLFW_KEY_S));
        world->UpdateChangeButtonState(player_soldier_id, Keyboard::Key(GLFW_KEY_Q));
        world->UpdateThrowGrenadeButtonState(player_soldier_id, Keyboard::Key(GLFW_KEY_E));
        world->UpdateDropButtonState(player_soldier_id, Keyboard::Key(GLFW_KEY_F));
        world->UpdateProneButtonState(player_soldier_id, Keyboard::Key(GLFW_KEY_X));

        world->UpdateMousePosition(player_soldier_id, { Mouse::GetX(), Mouse::GetY() });
    });
    world->SetPostGameLoopIterationCallback(
      [&](const std::shared_ptr<State>& state, double frame_percent, int last_fps) {
          scene.Render(state, world->GetSoldier(player_soldier_id), frame_percent, last_fps);

          window->SwapBuffers();
          window->PollInput();

          networking_client->Update();
      });

    const auto& soldier = world->CreateSoldier(197);
    player_soldier_id = soldier.id;
    std::cout << "Created soldier with id = " << player_soldier_id << std::endl;

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
