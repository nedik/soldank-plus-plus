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

void World::RunLoop(int fps_limit)
{
    std::chrono::time_point<std::chrono::system_clock> last_frame_time;
    auto last_fps_check_time = std::chrono::system_clock::now();
    int frame_count_since_last_fps_check = 0;
    int last_fps = 0;

    auto timecur = std::chrono::system_clock::now();
    auto timeprv = timecur;
    std::chrono::duration<double> timeacc{ 0 };

    int world_updates = 0;
    auto should_run_game_loop_iteration = [&]() {
        if (should_stop_game_loop_callback_) {
            return !should_stop_game_loop_callback_();
        }
        return true;
    };
    while (should_run_game_loop_iteration()) {
        if (pre_game_loop_iteration_callback_) {
            pre_game_loop_iteration_callback_();
        }

        auto current_frame_time = std::chrono::system_clock::now();
        std::chrono::duration<double> delta_time = current_frame_time - last_frame_time;
        last_frame_time = current_frame_time;

        frame_count_since_last_fps_check++;
        std::chrono::duration<double> diff = current_frame_time - last_fps_check_time;
        if (diff.count() >= 1.0) {
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

        timecur = std::chrono::system_clock::now();
        timeacc += (timecur - timeprv);
        timeprv = timecur;

        while (fps_limit != 0 && timeacc.count() < 1.0 / (float)fps_limit) {
            timecur = std::chrono::system_clock::now();
            timeacc += timecur - timeprv;
            timeprv = timecur;

            // TODO: Don't use sleep when VSync is on
            // Sleep for 0 milliseconds to give the resource to other processes
            std::this_thread::sleep_for(std::chrono::milliseconds(0));
        }

        if (pre_world_update_callback_) {
            pre_world_update_callback_();
        }

        while (timeacc.count() >= dt) {
            std::chrono::duration<double> dt_in_duration{ dt };
            timeacc -= dt_in_duration;

            world_updates++;

            Update(delta_time.count());

            timecur = std::chrono::system_clock::now();
            timeacc += timecur - timeprv;
            timeprv = timecur;
        }
        double frame_percent = std::min(1.0, std::max(0.0, timeacc.count() / dt));

        if (post_game_loop_iteration_callback_) {
            post_game_loop_iteration_callback_(state_, frame_percent, last_fps);
        }
    }
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