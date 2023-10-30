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

void Init()
{
    window = std::make_unique<Window>();
}

void UpdateMouseButton(int button, int action)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        World::UpdateFireButtonState(action == GLFW_PRESS);
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        World::UpdateJetsButtonState(action == GLFW_PRESS);
    }
}

void Run()
{
    window->Create();

    double delta_time = 0.0;
    double last_frame_time = 0.0;

    double last_fps_check_time = glfwGetTime();
    int frame_count_since_last_fps_check = 0;
    int last_fps = 0;

    double timecur = glfwGetTime();
    double timeprv = timecur;
    double timeacc = 0.0;

    int world_updates = 0;

    World::Init();
    Scene scene(World::GetState());
    Mouse::SubscribeButtonObserver(
      [](int button, int action) { UpdateMouseButton(button, action); });

    while (!window->ShouldClose()) {
        if (Keyboard::KeyWentDown(GLFW_KEY_ESCAPE)) {
            window->Close();
        }

        const double current_frame_time = glfwGetTime();
        delta_time = current_frame_time - last_frame_time;
        last_frame_time = current_frame_time;

        frame_count_since_last_fps_check++;
        if (current_frame_time - last_fps_check_time >= 1.0) {
            std::cout << 1000.0 / double(frame_count_since_last_fps_check) << " ms/frame"
                      << std::endl;
            std::cout << "FPS: " << frame_count_since_last_fps_check << std::endl;
            last_fps = frame_count_since_last_fps_check;
            frame_count_since_last_fps_check = 0;
            last_fps_check_time = current_frame_time;

            std::cout << "World updates: " << world_updates << std::endl;
            world_updates = 0;
        }

        double dt = 1.0 / 60.0;

        timecur = glfwGetTime();
        timeacc += timecur - timeprv;
        timeprv = timecur;

        while (Config::FPS_LIMIT != 0 && timeacc < 1.0 / (float)Config::FPS_LIMIT) {
            timecur = glfwGetTime();
            timeacc += timecur - timeprv;
            timeprv = timecur;

            // TODO: Don't use sleep when VSync is on
            // Sleep for 0 milliseconds to give the resource to other processes
            std::this_thread::sleep_for(std::chrono::milliseconds(0));
        }

        while (timeacc >= dt) {
            timeacc -= dt;

            world_updates++;

            World::UpdateLeftButtonState(Keyboard::Key(GLFW_KEY_A));
            World::UpdateRightButtonState(Keyboard::Key(GLFW_KEY_D));
            World::UpdateJumpButtonState(Keyboard::Key(GLFW_KEY_W));
            World::UpdateRightButtonState(Keyboard::Key(GLFW_KEY_D));
            World::UpdateCrouchButtonState(Keyboard::Key(GLFW_KEY_S));
            World::UpdateChangeButtonState(Keyboard::Key(GLFW_KEY_Q));
            World::UpdateThrowGrenadeButtonState(Keyboard::Key(GLFW_KEY_E));
            World::UpdateDropButtonState(Keyboard::Key(GLFW_KEY_F));
            World::UpdateProneButtonState(Keyboard::Key(GLFW_KEY_X));

            World::UpdateMousePosition({ Mouse::GetX(), Mouse::GetY() });

            World::Update(delta_time);

            timecur = glfwGetTime();
            timeacc += timecur - timeprv;
            timeprv = timecur;
        }
        double p = std::min(1.0, std::max(0.0, timeacc / dt));
        scene.Render(World::GetState(), World::GetSoldier(), p, last_fps);

        window->SwapBuffers();
        window->PollInput();
    }
}

void Free()
{
    World::Free();
    window.reset(nullptr);
}
} // namespace Soldat::Application
