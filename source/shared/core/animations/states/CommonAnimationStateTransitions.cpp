#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/animations/states/BodyThrowAnimationState.hpp"
#include "core/animations/states/LegsCrouchRunAnimationState.hpp"
#include "core/animations/states/LegsCrouchRunBackAnimationState.hpp"
#include "core/animations/states/LegsRollBackAnimationState.hpp"
#include "core/animations/states/LegsRunBackAnimationState.hpp"
#include "core/animations/states/LegsRunAnimationState.hpp"
#include "core/animations/states/LegsRollAnimationState.hpp"

#include "core/entities/Soldier.hpp"
#include <optional>

namespace Soldank::CommonAnimationStateTransitions
{
std::optional<std::shared_ptr<AnimationState>> TryTransitionToRunning(
  const Soldier& soldier,
  const AnimationDataManager& animation_data_manager)
{
    if (soldier.control.left) {
        if (soldier.direction == 1) {
            return std::make_shared<LegsRunBackAnimationState>(animation_data_manager);
        }

        return std::make_shared<LegsRunAnimationState>(animation_data_manager);
    }

    if (soldier.control.right) {
        if (soldier.direction == -1) {
            return std::make_shared<LegsRunBackAnimationState>(animation_data_manager);
        }

        return std::make_shared<LegsRunAnimationState>(animation_data_manager);
    }

    return std::nullopt;
}

std::optional<std::shared_ptr<AnimationState>> TryTransitionToCrouchRunning(
  const Soldier& soldier,
  const AnimationDataManager& animation_data_manager)
{
    if (soldier.control.right) {
        if (soldier.direction == 1) {
            return std::make_shared<LegsCrouchRunAnimationState>(animation_data_manager);
        }

        return std::make_shared<LegsCrouchRunBackAnimationState>(animation_data_manager);
    }

    if (soldier.control.left) {
        if (soldier.direction == -1) {
            return std::make_shared<LegsCrouchRunAnimationState>(animation_data_manager);
        }

        return std::make_shared<LegsCrouchRunBackAnimationState>(animation_data_manager);
    }

    return std::nullopt;
}

std::optional<std::shared_ptr<AnimationState>> TryTransitionToRolling(
  const Soldier& soldier,
  const AnimationDataManager& animation_data_manager)
{
    if (soldier.control.down && soldier.control.left && soldier.direction == -1) {
        return std::make_shared<LegsRollAnimationState>(animation_data_manager);
    }

    if (soldier.control.down && soldier.control.right && soldier.direction == 1) {
        return std::make_shared<LegsRollAnimationState>(animation_data_manager);
    }

    if (soldier.control.down && soldier.control.left && soldier.direction == 1) {
        return std::make_shared<LegsRollBackAnimationState>(animation_data_manager);
    }

    if (soldier.control.down && soldier.control.right && soldier.direction == -1) {
        return std::make_shared<LegsRollBackAnimationState>(animation_data_manager);
    }

    return std::nullopt;
}

std::optional<std::shared_ptr<AnimationState>> TryTransitionToThrowingGrenade(
  Soldier& soldier,
  const AnimationDataManager& animation_data_manager)
{
    if (!soldier.control.throw_grenade) {
        soldier.grenade_can_throw = true;
    }

    if (soldier.grenade_can_throw && soldier.control.throw_grenade) {
        return std::make_shared<BodyThrowAnimationState>(animation_data_manager);
    }

    return std::nullopt;
}
} // namespace Soldank::CommonAnimationStateTransitions
