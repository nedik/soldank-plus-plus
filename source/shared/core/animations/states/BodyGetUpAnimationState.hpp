#ifndef __BODY_GET_UP_ANIMATION_STATE_HPP__
#define __BODY_GET_UP_ANIMATION_STATE_HPP__

#include "core/animations/AnimationState.hpp"

#include "core/animations/AnimationData.hpp"

namespace Soldank
{
struct Soldier;

class BodyGetUpAnimationState final : public Soldank::AnimationState
{
public:
    BodyGetUpAnimationState(const AnimationDataManager& animation_data_manager);
    ~BodyGetUpAnimationState() override = default;

    std::optional<std::shared_ptr<AnimationState>> HandleInput(Soldier& soldier) final;

private:
    bool IsSoldierShootingPossible(const Soldier& /*soldier*/) const final { return true; }
    bool IsSoldierFlagThrowingPossible(const Soldier& /*soldier*/) const final { return true; };

    const AnimationDataManager& animation_data_manager_;
};
} // namespace Soldank

#endif
