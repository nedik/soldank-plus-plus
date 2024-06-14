#include "core/animations/states/LegsFallAnimationState.hpp"

#include "core/animations/states/LegsCrouchRunAnimationState.hpp"
#include "core/animations/states/LegsJumpAnimationState.hpp"
#include "core/animations/states/LegsStandAnimationState.hpp"
#include "core/animations/states/LegsRunBackAnimationState.hpp"
#include "core/animations/states/LegsRunAnimationState.hpp"
#include "core/animations/states/LegsProneAnimationState.hpp"
#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/physics/Constants.hpp"
#include "core/entities/Soldier.hpp"

namespace Soldank
{
LegsFallAnimationState::LegsFallAnimationState(const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::Fall))
    , animation_data_manager_(animation_data_manager)
{
}

std::optional<std::shared_ptr<AnimationState>> LegsFallAnimationState::HandleInput(Soldier& soldier)
{
    if (soldier.control.prone) {
        return std::make_shared<LegsProneAnimationState>(animation_data_manager_);
    }

    if (soldier.control.up && !soldier.on_ground) {
        return std::nullopt;
    }

    auto maybe_running_animation_state =
      CommonAnimationStateTransitions::TryTransitionToRunning(soldier, animation_data_manager_);
    if (maybe_running_animation_state.has_value()) {
        return *maybe_running_animation_state;
    }

    if (soldier.on_ground) {
        if (soldier.control.up) {
            return std::make_shared<LegsJumpAnimationState>(animation_data_manager_);
        }

        if (soldier.control.down) {
            return std::make_shared<LegsCrouchRunAnimationState>(animation_data_manager_);
        }

        return std::make_shared<LegsStandAnimationState>(animation_data_manager_);
    }

    return std::nullopt;
}

void LegsFallAnimationState::Update(Soldier& soldier, const PhysicsEvents& physics_events)
{
    soldier.stance = PhysicsConstants::STANCE_STAND;
}
} // namespace Soldank
