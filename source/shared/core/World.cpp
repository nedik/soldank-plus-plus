#include "World.hpp"

#include "core/state/Control.hpp"

#include "spdlog/spdlog.h"

#include <algorithm>
#include <random>
#include <ranges>
#include <memory>
#include <tuple>
#include <chrono>

namespace Soldat
{
World::World()
    : state_(std::make_shared<State>("maps/ctf_Ash.pms"))
    , soldier_physics_(std::make_unique<SoldierPhysics>())
    , bullet_physics_(std::make_unique<BulletPhysics>())
    , mersenne_twister_engine_(random_device_())
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

    unsigned int game_tick = 0;
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
            spdlog::info("{} ms/frame", 1000.0 / double(frame_count_since_last_fps_check));
            spdlog::info("FPS: {}", frame_count_since_last_fps_check);
            last_fps = frame_count_since_last_fps_check;
            frame_count_since_last_fps_check = 0;
            last_fps_check_time = current_frame_time;

            spdlog::info("World updates: {}", world_updates);
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

        while (timeacc.count() >= dt) {
            std::chrono::duration<double> dt_in_duration{ dt };
            timeacc -= dt_in_duration;

            if (pre_world_update_callback_) {
                pre_world_update_callback_();
            }
            spdlog::info("World update");
            Update(delta_time.count());
            if (post_world_update_callback_) {
                post_world_update_callback_(state_);
            }

            world_updates++;
            game_tick++;
            state_->game_tick = game_tick;

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
    auto removed_bullets_range =
      std::ranges::remove_if(state_->bullets, [](const Bullet& bullet) { return !bullet.active; });
    state_->bullets.erase(removed_bullets_range.begin(), removed_bullets_range.end());

    std::vector<BulletParams> bullet_emitter;

    for (auto& soldier : state_->soldiers) {
        if (soldier.active) {
            soldier_physics_->Update(*state_, soldier, bullet_emitter);
        }
    }

    for (auto& bullet : state_->bullets) {
        bullet_physics_->UpdateBullet(bullet, state_->map);
    }

    for (const auto& bullet_params : bullet_emitter) {
        state_->bullets.emplace_back(bullet_params);
    }
}

void World::UpdateSoldier(unsigned int soldier_id)
{
    std::vector<BulletParams> bullet_emitter;

    for (auto& soldier : state_->soldiers) {
        if (soldier.active && soldier.id == soldier_id) {
            soldier_physics_->Update(*state_, soldier, bullet_emitter);
        }
    }

    for (const auto& bullet_params : bullet_emitter) {
        state_->bullets.emplace_back(bullet_params);
    }
}

const std::shared_ptr<State>& World::GetState() const
{
    return state_;
}

const Soldier& World::GetSoldier(unsigned int soldier_id) const
{
    for (const auto& soldier : state_->soldiers) {
        if (soldier.id == soldier_id) {
            return soldier;
        }
    }

    std::unreachable();
}

const Soldier& World::CreateSoldier(std::optional<unsigned int> force_soldier_id)
{
    unsigned int new_soldier_id = NAN;

    if (!force_soldier_id.has_value()) {
        std::vector<unsigned int> current_soldier_ids;
        for (const auto& soldier : state_->soldiers) {
            current_soldier_ids.push_back(soldier.id);
        }
        std::sort(current_soldier_ids.begin(), current_soldier_ids.end());
        unsigned int free_soldier_id = 0;
        while (free_soldier_id < current_soldier_ids.size() &&
               current_soldier_ids[free_soldier_id] == free_soldier_id) {
            free_soldier_id++;
        }

        new_soldier_id = free_soldier_id;
    } else {
        new_soldier_id = *force_soldier_id;
    }

    std::uniform_int_distribution<unsigned int> spawnpoint_id_random_distribution(
      0, state_->map.GetSpawnPoints().size() - 1);

    unsigned int random_spawnpoint_id = spawnpoint_id_random_distribution(mersenne_twister_engine_);

    const auto& chosen_spawnpoint = state_->map.GetSpawnPoints().at(random_spawnpoint_id);
    glm::vec2 spawn_position = { chosen_spawnpoint.x, chosen_spawnpoint.y };

    state_->soldiers.emplace_back(new_soldier_id, spawn_position);

    return state_->soldiers.back();
}

glm::vec2 World::SpawnSoldier(unsigned int soldier_id, std::optional<glm::vec2> spawn_position)
{
    glm::vec2 initial_player_position;
    if (spawn_position.has_value()) {
        initial_player_position = *spawn_position;
    } else {
        std::uniform_int_distribution<unsigned int> spawnpoint_id_random_distribution(
          0, state_->map.GetSpawnPoints().size() - 1);

        unsigned int random_spawnpoint_id =
          spawnpoint_id_random_distribution(mersenne_twister_engine_);

        const auto& chosen_spawnpoint = state_->map.GetSpawnPoints().at(random_spawnpoint_id);
        initial_player_position = { chosen_spawnpoint.x, chosen_spawnpoint.y };
    }

    for (auto& soldier : state_->soldiers) {
        if (soldier.id == soldier_id) {
            soldier.particle.position = initial_player_position;
            soldier.particle.old_position = initial_player_position;
            soldier.active = true;
            soldier.particle.active = true;
            return initial_player_position;
        }
    }

    spdlog::critical("[SpawnSoldier] Wrong soldier_id ({})", soldier_id);
    std::unreachable();
}

void World::UpdateFireButtonState(unsigned int soldier_id, bool pressed)
{
    for (auto& soldier_it : state_->soldiers) {
        if (soldier_it.id == soldier_id) {
            soldier_it.control.fire = pressed;
            return;
        }
    }
    spdlog::error("Wrong soldier_id({}) in UpdateFireButtonState", soldier_id);
}

void World::UpdateJetsButtonState(unsigned int soldier_id, bool pressed)
{
    for (auto& soldier_it : state_->soldiers) {
        if (soldier_it.id == soldier_id) {
            soldier_it.control.jets = pressed;
            return;
        }
    }
    spdlog::error("Wrong soldier_id({}) in UpdateJetsButtonState", soldier_id);
}

void World::UpdateLeftButtonState(unsigned int soldier_id, bool pressed)
{
    for (auto& soldier_it : state_->soldiers) {
        if (soldier_it.id == soldier_id) {
            soldier_it.control.left = pressed;
            return;
        }
    }
    spdlog::error("Wrong soldier_id({}) in UpdateLeftButtonState", soldier_id);
}

void World::UpdateRightButtonState(unsigned int soldier_id, bool pressed)
{
    for (auto& soldier_it : state_->soldiers) {
        if (soldier_it.id == soldier_id) {
            soldier_it.control.right = pressed;
            return;
        }
    }
    spdlog::error("Wrong soldier_id({}) in UpdateRightButtonState", soldier_id);
}

void World::UpdateJumpButtonState(unsigned int soldier_id, bool pressed)
{
    for (auto& soldier_it : state_->soldiers) {
        if (soldier_it.id == soldier_id) {
            soldier_it.control.up = pressed;
            return;
        }
    }
    spdlog::error("Wrong soldier_id({}) in UpdateJumpButtonState", soldier_id);
}

void World::UpdateCrouchButtonState(unsigned int soldier_id, bool pressed)
{
    for (auto& soldier_it : state_->soldiers) {
        if (soldier_it.id == soldier_id) {
            soldier_it.control.down = pressed;
            return;
        }
    }
    spdlog::error("Wrong soldier_id({}) in UpdateCrouchButtonState", soldier_id);
}

void World::UpdateProneButtonState(unsigned int soldier_id, bool pressed)
{
    for (auto& soldier_it : state_->soldiers) {
        if (soldier_it.id == soldier_id) {
            soldier_it.control.prone = pressed;
            return;
        }
    }
    spdlog::error("Wrong soldier_id({}) in UpdateProneButtonState", soldier_id);
}

void World::UpdateChangeButtonState(unsigned int soldier_id, bool pressed)
{
    for (auto& soldier_it : state_->soldiers) {
        if (soldier_it.id == soldier_id) {
            soldier_it.control.change = pressed;
            return;
        }
    }
    spdlog::error("Wrong soldier_id({}) in UpdateChangeButtonState", soldier_id);
}

void World::UpdateThrowGrenadeButtonState(unsigned int soldier_id, bool pressed)
{
    for (auto& soldier_it : state_->soldiers) {
        if (soldier_it.id == soldier_id) {
            soldier_it.control.throw_grenade = pressed;
            return;
        }
    }
    spdlog::error("Wrong soldier_id({}) in UpdateThrowGrenadeButtonState", soldier_id);
}

void World::UpdateDropButtonState(unsigned int soldier_id, bool pressed)
{
    for (auto& soldier_it : state_->soldiers) {
        if (soldier_it.id == soldier_id) {
            soldier_it.control.drop = pressed;
            return;
        }
    }
    spdlog::error("Wrong soldier_id({}) in UpdateDropButtonState", soldier_id);
}

void World::UpdateMousePosition(unsigned int soldier_id, glm::vec2 mouse_position)
{
    for (auto& soldier : state_->soldiers) {
        if (soldier.id == soldier_id) {
            soldier.game_width = 640.0;
            soldier.game_height = 480.0;
            soldier.camera_prev = soldier.camera;

            soldier.mouse.x = mouse_position.x;
            soldier.mouse.y =
              480.0F - mouse_position.y; // TODO: soldier.control.mouse_aim_y expects
                                         // top to be 0 and bottom to be game_height

            soldier.camera.x =
              soldier.particle.position.x + (float)(soldier.mouse.x - (soldier.game_width / 2));
            soldier.camera.y = soldier.particle.position.y -
                               (float)((480.0F - soldier.mouse.y) - (soldier.game_height / 2));

            return;
        }
    }

    spdlog::error("Wrong soldier_id({}) in UpdateMousePosition", soldier_id);
}
} // namespace Soldat