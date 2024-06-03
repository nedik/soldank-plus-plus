#ifndef __LEGS_JUMP_SIDE_ANIMATION_STATE_HPP__
#define __LEGS_JUMP_SIDE_ANIMATION_STATE_HPP__

#include "core/animations/AnimationState.hpp"

#include "core/animations/AnimationData.hpp"

namespace Soldank
{
struct Soldier;

class LegsJumpSideAnimationState final : public Soldank::AnimationState
{
public:
    LegsJumpSideAnimationState(const AnimationDataManager& animation_data_manager);
    ~LegsJumpSideAnimationState() override = default;

    void Enter(Soldier& soldier) final;
    std::optional<std::shared_ptr<AnimationState>> HandleInput(Soldier& soldier) final;
    void Update(Soldier& soldier, const PhysicsEvents& physics_events) final;

    void SetInitialJumpingDirection(bool jumping_direction_left);

private:
    const AnimationDataManager& animation_data_manager_;

    bool jumping_direction_left_{ false };
    bool both_direction_keys_pressed_{ false };
};
} // namespace Soldank

#endif
