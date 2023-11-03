#include "World.hpp"

#include "core/state/Control.hpp"

#include <algorithm>
#include <ranges>
#include <memory>
#include <tuple>
#include <chrono>
#include <iostream>

namespace Soldat
{
namespace
{

} // namespace

World::World()
    : state_(std::make_shared<State>("maps/ctf_Ash.pms"))
    , soldier_(std::make_unique<Soldier>(
        glm::vec2(state_->map.GetSpawnPoints()[0].x, state_->map.GetSpawnPoints()[0].y)))
    , bullet_physics_(std::make_unique<BulletPhysics>())
{
}

void World::Update(double /*delta_time*/)
{
    state_->game_width = 640.0;
    state_->game_height = 480.0;
    state_->camera_prev = state_->camera;
    state_->camera.x =
      soldier_->particle.position.x + (float)(state_->mouse.x - (state_->game_width / 2));
    state_->camera.y = soldier_->particle.position.y -
                       (float)((480.0F - state_->mouse.y) - (state_->game_height / 2));

    auto removed_bullets_range =
      std::ranges::remove_if(state_->bullets, [](const Bullet& bullet) { return !bullet.active; });
    state_->bullets.erase(removed_bullets_range.begin(), removed_bullets_range.end());

    std::vector<BulletParams> bullet_emitter;
    soldier_->Update(*state_, bullet_emitter);

    for (auto& bullet : state_->bullets) {
        bullet_physics_->UpdateBullet(bullet, state_->map);
    }

    for (const auto& bullet_params : bullet_emitter) {
        state_->bullets.emplace_back(bullet_params);
    }
}

const std::shared_ptr<State>& World::GetState() const
{

    return state_;
}
const Soldier& World::GetSoldier() const
{
    return *soldier_;
}

void World::UpdateFireButtonState(bool pressed)
{
    soldier_->control.fire = pressed;
}

void World::UpdateJetsButtonState(bool pressed)
{
    soldier_->control.jets = pressed;
}

void World::UpdateLeftButtonState(bool pressed)
{
    soldier_->control.left = pressed;
}

void World::UpdateRightButtonState(bool pressed)
{
    soldier_->control.right = pressed;
}

void World::UpdateJumpButtonState(bool pressed)
{
    soldier_->control.up = pressed;
}

void World::UpdateCrouchButtonState(bool pressed)
{
    soldier_->control.down = pressed;
}

void World::UpdateProneButtonState(bool pressed)
{
    soldier_->control.prone = pressed;
}

void World::UpdateChangeButtonState(bool pressed)
{
    soldier_->control.change = pressed;
}

void World::UpdateThrowGrenadeButtonState(bool pressed)
{
    soldier_->control.throw_grenade = pressed;
}

void World::UpdateDropButtonState(bool pressed)
{
    soldier_->control.drop = pressed;
}

void World::UpdateMousePosition(glm::vec2 mouse_position)
{
    state_->mouse.x = mouse_position.x;
    state_->mouse.y = 480.0F - mouse_position.y; // TODO: soldier.control.mouse_aim_y expects top
                                                 // to be 0 and bottom to be game_height
}
} // namespace Soldat