#ifndef __LEGS_FALL_ANIMATION_STATE_HPP__
#define __LEGS_FALL_ANIMATION_STATE_HPP__

#include "core/animations/AnimationState.hpp"

#include "core/animations/AnimationData.hpp"

namespace Soldank
{
struct Soldier;

class LegsFallAnimationState final : public Soldank::AnimationState
{
public:
    LegsFallAnimationState(const AnimationDataManager& animation_data_manager);
    ~LegsFallAnimationState() override = default;

    std::optional<std::shared_ptr<AnimationState>> HandleInput(Soldier& soldier) final;
    void Update(Soldier& soldier, const PhysicsEvents& physics_events) final;

private:
    const AnimationDataManager& animation_data_manager_;
};
} // namespace Soldank

#endif
