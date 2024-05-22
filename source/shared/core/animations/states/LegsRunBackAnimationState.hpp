#ifndef __LEGS_RUN_BACK_ANIMATION_STATE_HPP__
#define __LEGS_RUN_BACK_ANIMATION_STATE_HPP__

#include "core/animations/AnimationState.hpp"

#include "core/animations/AnimationData.hpp"

namespace Soldank
{
struct Soldier;

class LegsRunBackAnimationState final : public Soldank::AnimationState
{
public:
    LegsRunBackAnimationState(const AnimationDataManager& animation_data_manager,
                              bool was_holding_left,
                              bool was_holding_right);
    ~LegsRunBackAnimationState() override = default;

    void HandleInput(Soldier& soldier) final;
    void Update(Soldier& soldier) final;

private:
    const AnimationDataManager& animation_data_manager_;

    bool was_holding_left_;
    bool was_holding_right_;
};
} // namespace Soldank

#endif
