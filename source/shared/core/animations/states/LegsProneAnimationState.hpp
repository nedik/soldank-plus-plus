#ifndef __LEGS_PRONE_ANIMATION_STATE_HPP__
#define __LEGS_PRONE_ANIMATION_STATE_HPP__

#include "core/animations/AnimationState.hpp"

#include "core/animations/AnimationData.hpp"

namespace Soldank
{
struct Soldier;

class LegsProneAnimationState final : public Soldank::AnimationState
{
public:
    LegsProneAnimationState(const AnimationDataManager& animation_data_manager);
    ~LegsProneAnimationState() override = default;

    std::optional<std::shared_ptr<AnimationState>> HandleInput(Soldier& soldier) final;
    void Update(Soldier& soldier) final;

private:
    const AnimationDataManager& animation_data_manager_;
};
} // namespace Soldank

#endif
