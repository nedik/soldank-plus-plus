#ifndef __BODY_PRONE_ANIMATION_STATE_HPP__
#define __BODY_PRONE_ANIMATION_STATE_HPP__

#include "core/animations/AnimationState.hpp"

#include "core/animations/AnimationData.hpp"

namespace Soldank
{
struct Soldier;

class BodyProneAnimationState final : public Soldank::AnimationState
{
public:
    BodyProneAnimationState(const AnimationDataManager& animation_data_manager);
    ~BodyProneAnimationState() override = default;

    std::optional<std::shared_ptr<AnimationState>> HandleInput(Soldier& soldier) final;

private:
    bool IsSoldierShootingPossible(const Soldier& /*soldier*/) const final { return true; }
    bool IsSoldierFlagThrowingPossible(const Soldier& /*soldier*/) const final { return true; };

    const AnimationDataManager& animation_data_manager_;
};
} // namespace Soldank

#endif
