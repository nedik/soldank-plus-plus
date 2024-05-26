#ifndef __BODY_STAND_ANIMATION_STATE_HPP__
#define __BODY_STAND_ANIMATION_STATE_HPP__

#include "core/animations/AnimationState.hpp"

#include "core/animations/AnimationData.hpp"

namespace Soldank
{
struct Soldier;

class BodyStandAnimationState final : public Soldank::AnimationState
{
public:
    BodyStandAnimationState(const AnimationDataManager& animation_data_manager);
    ~BodyStandAnimationState() override = default;

    std::optional<std::shared_ptr<AnimationState>> HandleInput(Soldier& soldier) final;

private:
    const AnimationDataManager& animation_data_manager_;
};
} // namespace Soldank

#endif
