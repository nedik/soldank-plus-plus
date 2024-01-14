#ifndef __WORLD_HPP__
#define __WORLD_HPP__

#include "core/state/State.hpp"
#include "core/physics/SoldierPhysics.hpp"
#include "core/physics/BulletPhysics.hpp"
#include "core/entities/Soldier.hpp"

#include <functional>
#include <utility>
#include <random>
#include <optional>

namespace Soldat
{
class World
{
private:
    using TShouldStopGameLoopCallback = std::function<bool()>;
    using TPreGameLoopIterationCallback = std::function<void()>;
    using TPreWorldUpdateCallback = std::function<void()>;
    using TPostGameLoopIterationCallback =
      std::function<void(const std::shared_ptr<State>&, double, int)>;

public:
    World();

    void RunLoop(int fps_limit);
    void Update(double delta_time);
    const std::shared_ptr<State>& GetState() const;
    const Soldier& GetSoldier(unsigned int soldier_id) const;

    const Soldier& CreateSoldier(std::optional<unsigned int> force_soldier_id = std::nullopt);
    glm::vec2 SpawnSoldier(unsigned int soldier_id,
                           std::optional<glm::vec2> spawn_position = std::nullopt);

    void UpdateFireButtonState(unsigned int soldier_id, bool pressed);
    void UpdateJetsButtonState(unsigned int soldier_id, bool pressed);
    void UpdateLeftButtonState(unsigned int soldier_id, bool pressed);
    void UpdateRightButtonState(unsigned int soldier_id, bool pressed);
    void UpdateJumpButtonState(unsigned int soldier_id, bool pressed);
    void UpdateCrouchButtonState(unsigned int soldier_id, bool pressed);
    void UpdateProneButtonState(unsigned int soldier_id, bool pressed);
    void UpdateChangeButtonState(unsigned int soldier_id, bool pressed);
    void UpdateThrowGrenadeButtonState(unsigned int soldier_id, bool pressed);
    void UpdateDropButtonState(unsigned int soldier_id, bool pressed);
    void UpdateMousePosition(unsigned int soldier_id, glm::vec2 mouse_position);

    void SetShouldStopGameLoopCallback(TShouldStopGameLoopCallback callback)
    {
        should_stop_game_loop_callback_ = std::move(callback);
    }

    void SetPreGameLoopIterationCallback(TPreGameLoopIterationCallback callback)
    {
        pre_game_loop_iteration_callback_ = std::move(callback);
    }

    void SetPreWorldUpdateCallback(TPreWorldUpdateCallback callback)
    {
        pre_world_update_callback_ = std::move(callback);
    }

    void SetPostGameLoopIterationCallback(TPostGameLoopIterationCallback callback)
    {
        post_game_loop_iteration_callback_ = std::move(callback);
    }

private:
    std::shared_ptr<State> state_;
    std::unique_ptr<SoldierPhysics> soldier_physics_;
    std::unique_ptr<BulletPhysics> bullet_physics_;

    TShouldStopGameLoopCallback should_stop_game_loop_callback_;
    TPreGameLoopIterationCallback pre_game_loop_iteration_callback_;
    TPreWorldUpdateCallback pre_world_update_callback_;
    TPostGameLoopIterationCallback post_game_loop_iteration_callback_;

    std::random_device random_device_;
    std::mt19937 mersenne_twister_engine_;
};

} // namespace Soldat

#endif
