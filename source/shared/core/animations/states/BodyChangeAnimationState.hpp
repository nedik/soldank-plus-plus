#ifndef __BODY_CHANGE_ANIMATION_STATE_HPP__
#define __BODY_CHANGE_ANIMATION_STATE_HPP__

#include "core/animations/AnimationState.hpp"

#include "core/animations/AnimationData.hpp"

namespace Soldank
{
struct Soldier;

class BodyChangeAnimationState final : public Soldank::AnimationState
{
public:
    BodyChangeAnimationState(const AnimationDataManager& animation_data_manager);
    ~BodyChangeAnimationState() final = default;

    std::optional<std::shared_ptr<AnimationState>> HandleInput(Soldier& soldier) final;
    void Update(Soldier& soldier) final;

private:
    const AnimationDataManager& animation_data_manager_;
};
} // namespace Soldank

#endif
