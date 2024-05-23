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

std::optional<std::shared_ptr<AnimationState>> LegsStandAnimationState::HandleInput(
  Soldier& soldier)
{
    if (soldier.control.left) {
        if (soldier.direction == 1) {
            return std::make_shared<LegsRunBackAnimationState>(
              animation_data_manager_, soldier.control.left, soldier.control.right);
        }

        return std::make_shared<LegsRunAnimationState>(
          animation_data_manager_, soldier.control.left, soldier.control.right);
    }

    if (soldier.control.right) {
        if (soldier.direction == -1) {
            return std::make_shared<LegsRunBackAnimationState>(
              animation_data_manager_, soldier.control.left, soldier.control.right);
        }

        return std::make_shared<LegsRunAnimationState>(
          animation_data_manager_, soldier.control.left, soldier.control.right);
    }
    return std::nullopt;
}

void LegsStandAnimationState::Update(Soldier& soldier)
{
    soldier.stance = 1;
}
} // namespace Soldank
