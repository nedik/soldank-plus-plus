#ifndef __LEGS_RUN_BACK_ANIMATION_STATE_HPP__
#define __LEGS_RUN_BACK_ANIMATION_STATE_HPP__

#include "core/animations/AnimationState.hpp"

#include "core/animations/AnimationData.hpp"

namespace Soldank
{
struct Soldier;

class LegsRunBackAnimationState final : public Soldank::AnimationState
{
public:
    LegsRunBackAnimationState(const AnimationDataManager& animation_data_manager);
    ~LegsRunBackAnimationState() override = default;

    void Enter(Soldier& soldier) final;
    std::optional<std::shared_ptr<AnimationState>> HandleInput(Soldier& soldier) final;
    void Update(Soldier& soldier, const PhysicsEvents& physics_events) final;

private:
    bool IsRunningLeft(const Soldier& soldier) const;

    const AnimationDataManager& animation_data_manager_;

    bool was_holding_left_{ false };
    bool was_holding_right_{ false };
};
} // namespace Soldank

#endif
