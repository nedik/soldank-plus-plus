#ifndef __BODY_ROLL_BACK_ANIMATION_STATE_HPP__
#define __BODY_ROLL_BACK_ANIMATION_STATE_HPP__

#include "core/animations/AnimationState.hpp"

#include "core/animations/AnimationData.hpp"

namespace Soldank
{
struct Soldier;

class BodyRollBackAnimationState final : public Soldank::AnimationState
{
public:
    BodyRollBackAnimationState(const AnimationDataManager& animation_data_manager);
    ~BodyRollBackAnimationState() override = default;

    std::optional<std::shared_ptr<AnimationState>> HandleInput(Soldier& soldier) final;

private:
    bool IsSoldierShootingPossible(const Soldier& soldier) const final;

    const AnimationDataManager& animation_data_manager_;
};
} // namespace Soldank

#endif
