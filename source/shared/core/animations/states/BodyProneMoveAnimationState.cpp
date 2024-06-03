#include "core/animations/states/BodyProneMoveAnimationState.hpp"

#include "core/animations/states/BodyChangeAnimationState.hpp"
#include "core/animations/states/BodyGetUpAnimationState.hpp"
#include "core/animations/states/BodyProneAnimationState.hpp"
#include "core/animations/states/BodyRollAnimationState.hpp"
#include "core/animations/states/BodyRollBackAnimationState.hpp"
#include "core/animations/states/BodyThrowWeaponAnimationState.hpp"

#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/animations/AnimationData.hpp"
#include "core/entities/Soldier.hpp"
#include "core/physics/Constants.hpp"

namespace Soldank
{
BodyProneMoveAnimationState::BodyProneMoveAnimationState(
  const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::ProneMove))
    , animation_data_manager_(animation_data_manager)
{
}

std::optional<std::shared_ptr<AnimationState>> BodyProneMoveAnimationState::HandleInput(
  Soldier& soldier)
{
    if (soldier.control.prone || soldier.direction != soldier.old_direction) {
        auto new_state = std::make_shared<BodyGetUpAnimationState>(animation_data_manager_);
        new_state->SetFrame(9);
        return new_state;
    }

    if (soldier.on_ground) {
        if (soldier.control.down && (soldier.control.left || soldier.control.right)) {
            if (soldier.legs_animation->GetType() == AnimationType::Roll) {
                return std::make_shared<BodyRollAnimationState>(animation_data_manager_);
            }

            if (soldier.legs_animation->GetType() == AnimationType::RollBack) {
                return std::make_shared<BodyRollBackAnimationState>(animation_data_manager_);
            }
        }
    }

    if (soldier.control.change) {
        return std::make_shared<BodyChangeAnimationState>(animation_data_manager_);
    }

    if (soldier.control.drop &&
        soldier.weapons[0].GetWeaponParameters().kind != WeaponType::NoWeapon) {
        return std::make_shared<BodyThrowWeaponAnimationState>(animation_data_manager_);
    }

    auto maybe_throw_grenade_animation_state =
      CommonAnimationStateTransitions::TryTransitionToThrowingGrenade(soldier,
                                                                      animation_data_manager_);
    if (maybe_throw_grenade_animation_state.has_value()) {
        return *maybe_throw_grenade_animation_state;
    }

    if (!soldier.control.left && !soldier.control.right) {
        auto new_state = std::make_shared<BodyProneAnimationState>(animation_data_manager_);
        new_state->SetFrame(26);
        return new_state;
    }

    if (GetFrame() == GetFramesCount()) {
        auto new_state = std::make_shared<BodyProneAnimationState>(animation_data_manager_);
        new_state->SetFrame(26);
        return new_state;
    }

    return std::nullopt;
}
} // namespace Soldank
