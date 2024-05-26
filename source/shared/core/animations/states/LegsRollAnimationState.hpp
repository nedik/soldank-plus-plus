#ifndef __LEGS_ROLL_ANIMATION_STATE_HPP__
#define __LEGS_ROLL_ANIMATION_STATE_HPP__

#include "core/animations/AnimationState.hpp"

#include "core/animations/AnimationData.hpp"

namespace Soldank
{
struct Soldier;

class LegsRollAnimationState final : public Soldank::AnimationState
{
public:
    LegsRollAnimationState(const AnimationDataManager& animation_data_manager);
    ~LegsRollAnimationState() override = default;

    void Enter(Soldier& soldier) final;
    std::optional<std::shared_ptr<AnimationState>> HandleInput(Soldier& soldier) final;
    void Update(Soldier& soldier, const PhysicsEvents& physics_events) final;

private:
    const AnimationDataManager& animation_data_manager_;
};
} // namespace Soldank

#endif
