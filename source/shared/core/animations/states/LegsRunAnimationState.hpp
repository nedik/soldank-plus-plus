#ifndef __LEGS_RUN_ANIMATION_STATE_HPP__
#define __LEGS_RUN_ANIMATION_STATE_HPP__

#include "core/animations/AnimationState.hpp"

#include "core/animations/AnimationData.hpp"

namespace Soldank
{
struct Soldier;

class LegsRunAnimationState final : public Soldank::AnimationState
{
public:
    LegsRunAnimationState(const AnimationDataManager& animation_data_manager,
                          bool was_holding_left,
                          bool was_holding_right);
    ~LegsRunAnimationState() override = default;

    std::optional<std::shared_ptr<AnimationState>> HandleInput(Soldier& soldier) final;
    void Update(Soldier& soldier, const PhysicsEvents& physics_events) final;

private:
    const AnimationDataManager& animation_data_manager_;

    bool was_holding_left_;
    bool was_holding_right_;
};
} // namespace Soldank

#endif
