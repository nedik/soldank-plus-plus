#include "core/animations/states/LegsStandAnimationState.hpp"

#include "core/animations/states/LegsCrouchAnimationState.hpp"
#include "core/animations/states/LegsProneAnimationState.hpp"
#include "core/animations/states/LegsRunBackAnimationState.hpp"
#include "core/animations/states/LegsRunAnimationState.hpp"
#include "core/animations/states/LegsFallAnimationState.hpp"
#include "core/animations/states/LegsJumpAnimationState.hpp"
#include "core/animations/states/LegsJumpSideAnimationState.hpp"

#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/animations/AnimationData.hpp"
#include "core/entities/Soldier.hpp"

namespace Soldank
{
LegsStandAnimationState::LegsStandAnimationState(const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::Stand))
    , animation_data_manager_(animation_data_manager)
{
}

std::optional<std::shared_ptr<AnimationState>> LegsStandAnimationState::HandleInput(
  Soldier& soldier)
{
    if (soldier.control.prone) {
        return std::make_shared<LegsProneAnimationState>(animation_data_manager_);
    }

    if (soldier.control.up && soldier.on_ground) {
        if (soldier.control.left || soldier.control.right) {
            return std::make_shared<LegsJumpSideAnimationState>(animation_data_manager_);
        }
        return std::make_shared<LegsJumpAnimationState>(animation_data_manager_);
    }

    auto maybe_running_animation_state =
      CommonAnimationStateTransitions::TryTransitionToRunning(soldier, animation_data_manager_);
    if (maybe_running_animation_state.has_value()) {
        return *maybe_running_animation_state;
    }

    if (!soldier.on_ground) {
        return std::make_shared<LegsFallAnimationState>(animation_data_manager_);
    }

    if (soldier.control.down) {
        return std::make_shared<LegsCrouchAnimationState>(animation_data_manager_);
    }

    return std::nullopt;
}

void LegsStandAnimationState::Update(Soldier& soldier)
{
    soldier.stance = 1;
}
} // namespace Soldank
