#ifndef __WORLD_HPP__
#define __WORLD_HPP__

#include "core/state/State.hpp"
#include "core/physics/SoldierPhysics.hpp"
#include "core/physics/BulletPhysics.hpp"
#include "core/entities/Soldier.hpp"

#include <functional>
#include <utility>

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
    const Soldier& GetSoldier() const;

    void UpdateFireButtonState(bool pressed);
    void UpdateJetsButtonState(bool pressed);
    void UpdateLeftButtonState(bool pressed);
    void UpdateRightButtonState(bool pressed);
    void UpdateJumpButtonState(bool pressed);
    void UpdateCrouchButtonState(bool pressed);
    void UpdateProneButtonState(bool pressed);
    void UpdateChangeButtonState(bool pressed);
    void UpdateThrowGrenadeButtonState(bool pressed);
    void UpdateDropButtonState(bool pressed);
    void UpdateMousePosition(glm::vec2 mouse_position);

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
    std::unique_ptr<Soldier> soldier_;

    TShouldStopGameLoopCallback should_stop_game_loop_callback_;
    TPreGameLoopIterationCallback pre_game_loop_iteration_callback_;
    TPreWorldUpdateCallback pre_world_update_callback_;
    TPostGameLoopIterationCallback post_game_loop_iteration_callback_;
};

} // namespace Soldat

#endif
