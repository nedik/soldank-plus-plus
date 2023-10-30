#include "World.hpp"

#include "core/state/Control.hpp"
#include "core/physics/BulletPhysics.hpp"

#include <algorithm>
#include <ranges>
#include <memory>
#include <tuple>

namespace Soldat::World
{
namespace
{
std::shared_ptr<State> state;
std::unique_ptr<Soldier> soldier;
std::unique_ptr<BulletPhysics> bullet_physics;
} // namespace

void Init()
{
    state = std::make_shared<State>("maps/ctf_Ash.pms");
    soldier = std::make_unique<Soldier>(
      glm::vec2(state->map.GetSpawnPoints()[0].x, state->map.GetSpawnPoints()[0].y));
    bullet_physics = std::make_unique<BulletPhysics>();
}

void Update(double /*delta_time*/)
{
    state->game_width = 640.0;
    state->game_height = 480.0;
    state->camera_prev = state->camera;
    state->camera.x =
      soldier->particle.position.x + (float)(state->mouse.x - (state->game_width / 2));
    state->camera.y =
      soldier->particle.position.y - (float)((480.0F - state->mouse.y) - (state->game_height / 2));

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

const std::shared_ptr<State>& GetState()
{

    return state;
}
const Soldier& GetSoldier()
{
    return *soldier;
}

void UpdateFireButtonState(bool pressed)
{
    soldier->control.fire = pressed;
}

void UpdateJetsButtonState(bool pressed)
{
    soldier->control.jets = pressed;
}

void UpdateLeftButtonState(bool pressed)
{
    soldier->control.left = pressed;
}

void UpdateRightButtonState(bool pressed)
{
    soldier->control.right = pressed;
}

void UpdateJumpButtonState(bool pressed)
{
    soldier->control.up = pressed;
}

void UpdateCrouchButtonState(bool pressed)
{
    soldier->control.down = pressed;
}

void UpdateProneButtonState(bool pressed)
{
    soldier->control.prone = pressed;
}

void UpdateChangeButtonState(bool pressed)
{
    soldier->control.change = pressed;
}

void UpdateThrowGrenadeButtonState(bool pressed)
{
    soldier->control.throw_grenade = pressed;
}

void UpdateDropButtonState(bool pressed)
{
    soldier->control.drop = pressed;
}

void UpdateMousePosition(glm::vec2 mouse_position)
{
    state->mouse.x = mouse_position.x;
    state->mouse.y = 480.0F - mouse_position.y; // TODO: soldier.control.mouse_aim_y expects top
                                                // to be 0 and bottom to be game_height
}

void Free()
{
    soldier.reset(nullptr);
    state.reset();
}
} // namespace Soldat::World