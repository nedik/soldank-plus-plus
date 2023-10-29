#include "World.hpp"

#include "core/state/State.hpp"
#include "core/state/Control.hpp"
#include "core/physics/Soldier.hpp"
#include "core/physics/BulletPhysics.hpp"

#include "application/input/Keyboard.hpp"
#include "application/input/Mouse.hpp"
#include "application/config/Config.hpp"

#include "rendering/Scene.hpp"

#include <GLFW/glfw3.h>

#include <memory>
#include <ranges>
#include <tuple>

namespace Soldat::World
{
namespace
{
std::shared_ptr<State> state;
std::unique_ptr<Scene> scene;
std::unique_ptr<Soldier> soldier;
std::unique_ptr<BulletPhysics> bullet_physics;
} // namespace

void UpdateMouseButton(int button, int action)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        soldier->control.fire = action == GLFW_PRESS;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        soldier->control.jets = action == GLFW_PRESS;
    }
}

void Init()
{
    state = std::make_shared<State>("maps/ctf_Ash.pms");
    scene = std::make_unique<Scene>(state);
    soldier = std::make_unique<Soldier>(
      glm::vec2(state->map.GetSpawnPoints()[0].x, state->map.GetSpawnPoints()[0].y));
    bullet_physics = std::make_unique<BulletPhysics>();
    Mouse::SubscribeButtonObserver(UpdateMouseButton);
}

void UpdateControlFromUserInput()
{
    soldier->control.left = Keyboard::Key(GLFW_KEY_A);
    soldier->control.right = Keyboard::Key(GLFW_KEY_D);
    soldier->control.up = Keyboard::Key(GLFW_KEY_W);
    soldier->control.down = Keyboard::Key(GLFW_KEY_S);
    soldier->control.change = Keyboard::Key(GLFW_KEY_Q);
    soldier->control.throw_grenade = Keyboard::Key(GLFW_KEY_E);
    soldier->control.drop = Keyboard::Key(GLFW_KEY_F);
    soldier->control.prone = Keyboard::Key(GLFW_KEY_X);
}

void Update(double /*delta_time*/)
{
    if (Config::DEBUG_DRAW) {
        state->colliding_polygon_ids.clear();
    }
    UpdateControlFromUserInput();
    state->mouse.x = (float)Mouse::GetX();
    state->mouse.y = 480.0F - (float)Mouse::GetY(); // TODO: soldier.control.mouse_aim_y expects top
                                                    // to be 0 and bottom to be game_height
    state->game_width = 640.0;
    state->game_height = 480.0;
    state->camera_prev = state->camera;
    state->camera.x =
      soldier->particle.position.x + (float)(Mouse::GetX() - (state->game_width / 2));
    state->camera.y =
      soldier->particle.position.y - (float)(Mouse::GetY() - (state->game_height / 2));

    auto removed_bullets_range =
      std::ranges::remove_if(state->bullets, [](const Bullet& bullet) { return !bullet.active; });
    state->bullets.erase(removed_bullets_range.begin(), removed_bullets_range.end());

    std::vector<BulletParams> bullet_emitter;
    soldier->Update(*state, bullet_emitter);

    for (auto& bullet : state->bullets) {
        bullet_physics->UpdateBullet(bullet, state->map);
    }

    for (const auto& bullet_params : bullet_emitter) {
        state->bullets.emplace_back(bullet_params);
    }
}

void Render(double frame_percent, int fps)
{
    state->mouse.x = (float)Mouse::GetX();
    state->mouse.y = 480.0F - (float)Mouse::GetY();
    scene->Render(state, *soldier, frame_percent, fps);
}

void Free()
{
    scene.reset(nullptr);
    soldier.reset(nullptr);
    state.reset();
}
} // namespace Soldat::World