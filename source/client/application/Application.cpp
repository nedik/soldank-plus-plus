#include "Application.hpp"

#include "application/config/Config.hpp"
#include "application/input/Keyboard.hpp"
#include "application/input/Mouse.hpp"

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
}

void UpdateMouseButton(int button, int action)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        world->UpdateFireButtonState(action == GLFW_PRESS);
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        world->UpdateJetsButtonState(action == GLFW_PRESS);
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
        world->UpdateLeftButtonState(Keyboard::Key(GLFW_KEY_A));
        world->UpdateRightButtonState(Keyboard::Key(GLFW_KEY_D));
        world->UpdateJumpButtonState(Keyboard::Key(GLFW_KEY_W));
        world->UpdateRightButtonState(Keyboard::Key(GLFW_KEY_D));
        world->UpdateCrouchButtonState(Keyboard::Key(GLFW_KEY_S));
        world->UpdateChangeButtonState(Keyboard::Key(GLFW_KEY_Q));
        world->UpdateThrowGrenadeButtonState(Keyboard::Key(GLFW_KEY_E));
        world->UpdateDropButtonState(Keyboard::Key(GLFW_KEY_F));
        world->UpdateProneButtonState(Keyboard::Key(GLFW_KEY_X));

        world->UpdateMousePosition({ Mouse::GetX(), Mouse::GetY() });
    });
    world->SetPostGameLoopIterationCallback(
      [&](const std::shared_ptr<State>& state, double frame_percent, int last_fps) {
          scene.Render(state, world->GetSoldier(), frame_percent, last_fps);

          window->SwapBuffers();
          window->PollInput();
      });

    world->RunLoop(Config::FPS_LIMIT);
}

void Free()
{
    window.reset(nullptr);

    // Give connections time to finish up.  This is an application layer protocol
    // here, it's not TCP.  Note that if you have an application and you need to be
    // more sure about cleanup, you won't be able to do this.  You will need to send
    // a message and then either wait for the peer to close the connection, or
    // you can pool the connection to see if any reliable data is pending.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    GameNetworkingSockets_Kill();
}
} // namespace Soldat::Application
