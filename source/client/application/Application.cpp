#include "Application.hpp"

#include "application/config/Config.hpp"
#include "application/input/Keyboard.hpp"
#include "application/input/Mouse.hpp"

#include "core/World.hpp"

#include "rendering/Scene.hpp"

#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

namespace Soldat::Application
{
std::unique_ptr<Window> window;
std::unique_ptr<World> world;

void Init()
{
    window = std::make_unique<Window>();
    world = std::make_unique<World>();
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
}
} // namespace Soldat::Application
