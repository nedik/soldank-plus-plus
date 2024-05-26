#ifndef __BODY_AIM_ANIMATION_STATE_HPP__
#define __BODY_AIM_ANIMATION_STATE_HPP__

#include "core/animations/AnimationState.hpp"

#include "core/animations/AnimationData.hpp"

namespace Soldank
{
struct Soldier;

class BodyAimAnimationState final : public Soldank::AnimationState
{
public:
    BodyAimAnimationState(const AnimationDataManager& animation_data_manager);
    ~BodyAimAnimationState() override = default;

    std::optional<std::shared_ptr<AnimationState>> HandleInput(Soldier& soldier) final;

private:
    const AnimationDataManager& animation_data_manager_;
};
} // namespace Soldank

#endif
