#ifndef __LEGS_PRONE_MOVE_ANIMATION_STATE_HPP__
#define __LEGS_PRONE_MOVE_ANIMATION_STATE_HPP__

#include "core/animations/AnimationState.hpp"

#include "core/animations/AnimationData.hpp"

namespace Soldank
{
struct Soldier;

class LegsProneMoveAnimationState final : public Soldank::AnimationState
{
public:
    LegsProneMoveAnimationState(const AnimationDataManager& animation_data_manager);
    ~LegsProneMoveAnimationState() override = default;

    void Enter(Soldier& soldier) final;
    std::optional<std::shared_ptr<AnimationState>> HandleInput(Soldier& soldier) final;
    void Update(Soldier& soldier, const PhysicsEvents& physics_events) final;

private:
    const AnimationDataManager& animation_data_manager_;

    bool moving_direction_left_{ false };
    bool both_direction_keys_pressed_{ false };
};
} // namespace Soldank

#endif
