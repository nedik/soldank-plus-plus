#include "core/animations/states/LegsStandAnimationState.hpp"

#include "core/animations/states/LegsRunBackAnimationState.hpp"
#include "core/animations/states/LegsRunAnimationState.hpp"

#include "core/animations/AnimationData.hpp"
#include "core/entities/Soldier.hpp"

namespace Soldank
{
LegsStandAnimationState::LegsStandAnimationState(const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::Stand))
    , animation_data_manager_(animation_data_manager)
{
}

void LegsStandAnimationState::HandleInput(Soldier& soldier)
{
    if (soldier.control.left) {
        if (soldier.direction == 1) {
            soldier.legs_animation_state_machine = std::make_unique<LegsRunBackAnimationState>(
              animation_data_manager_, soldier.control.left, soldier.control.right);
            return;
        }

        soldier.legs_animation_state_machine = std::make_unique<LegsRunAnimationState>(
          animation_data_manager_, soldier.control.left, soldier.control.right);
        return;
    }

    if (soldier.control.right) {
        if (soldier.direction == -1) {
            soldier.legs_animation_state_machine = std::make_unique<LegsRunBackAnimationState>(
              animation_data_manager_, soldier.control.left, soldier.control.right);
        } else {
            soldier.legs_animation_state_machine = std::make_unique<LegsRunAnimationState>(
              animation_data_manager_, soldier.control.left, soldier.control.right);
        }
    }
}

void LegsStandAnimationState::Update(Soldier& soldier)
{
    soldier.stance = 1;
}
} // namespace Soldank
