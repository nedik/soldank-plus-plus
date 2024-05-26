#include "core/animations/states/LegsProneAnimationState.hpp"

#include "core/animations/states/LegsGetUpAnimationState.hpp"
#include "core/animations/states/LegsProneMoveAnimationState.hpp"

#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/entities/Soldier.hpp"
#include "core/physics/Constants.hpp"

namespace Soldank
{
LegsProneAnimationState::LegsProneAnimationState(const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::Prone))
    , animation_data_manager_(animation_data_manager)
{
}

std::optional<std::shared_ptr<AnimationState>> LegsProneAnimationState::HandleInput(
  Soldier& soldier)
{
    // Set old direction when entering state
    if (GetFrame() == 2) {
        soldier.old_direction = soldier.direction;
    }

    if (GetFrame() > 25 && soldier.on_ground) {
        if (soldier.control.left || soldier.control.right) {
            return std::make_shared<LegsProneMoveAnimationState>(animation_data_manager_);
        }
    }

    if (GetFrame() > 23) {
        if (soldier.control.prone || soldier.direction != soldier.old_direction) {
            auto new_state = std::make_shared<LegsGetUpAnimationState>(animation_data_manager_);
            new_state->SetFrame(9);
            return new_state;
        }

        if (soldier.on_ground) {
            auto maybe_rolling_animation_state =
              CommonAnimationStateTransitions::TryTransitionToRolling(soldier,
                                                                      animation_data_manager_);
            if (maybe_rolling_animation_state.has_value()) {
                return *maybe_rolling_animation_state;
            }
        }
    }

    return std::nullopt;
}

void LegsProneAnimationState::Update(Soldier& soldier, const PhysicsEvents& physics_events)
{
    soldier.stance = PhysicsConstants::STANCE_PRONE;
}
} // namespace Soldank
