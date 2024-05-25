#ifndef __BODY_PRONE_MOVE_ANIMATION_STATE_HPP__
#define __BODY_PRONE_MOVE_ANIMATION_STATE_HPP__

#include "core/animations/AnimationState.hpp"

#include "core/animations/AnimationData.hpp"

namespace Soldank
{
struct Soldier;

class BodyProneMoveAnimationState final : public Soldank::AnimationState
{
public:
    BodyProneMoveAnimationState(const AnimationDataManager& animation_data_manager);
    ~BodyProneMoveAnimationState() override = default;

    std::optional<std::shared_ptr<AnimationState>> HandleInput(Soldier& soldier) final;
    void Update(Soldier& soldier) final;

private:
    const AnimationDataManager& animation_data_manager_;
};
} // namespace Soldank

#endif
