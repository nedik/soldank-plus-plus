#ifndef __LEGS_CROUCH_ANIMATION_STATE_HPP__
#define __LEGS_CROUCH_ANIMATION_STATE_HPP__

#include "core/animations/AnimationState.hpp"

#include "core/animations/AnimationData.hpp"

namespace Soldank
{
struct Soldier;

class LegsCrouchAnimationState final : public Soldank::AnimationState
{
public:
    LegsCrouchAnimationState(const AnimationDataManager& animation_data_manager);
    ~LegsCrouchAnimationState() override = default;

    std::optional<std::shared_ptr<AnimationState>> HandleInput(Soldier& soldier) final;
    void Update(Soldier& soldier, const PhysicsEvents& physics_events) final;

private:
    const AnimationDataManager& animation_data_manager_;
};
} // namespace Soldank

#endif
