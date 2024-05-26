#include "core/animations/states/BodyStandAnimationState.hpp"

#include "core/animations/states/BodyAimAnimationState.hpp"
#include "core/animations/states/BodyChangeAnimationState.hpp"
#include "core/animations/states/BodyProneAnimationState.hpp"
#include "core/animations/states/BodyPunchAnimationState.hpp"
#include "core/animations/states/BodyRollAnimationState.hpp"
#include "core/animations/states/BodyRollBackAnimationState.hpp"
#include "core/animations/states/BodyThrowAnimationState.hpp"
#include "core/animations/states/BodyThrowWeaponAnimationState.hpp"

#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/animations/AnimationData.hpp"
#include "core/entities/Soldier.hpp"
#include "core/physics/Constants.hpp"
#include <memory>

namespace Soldank
{
BodyStandAnimationState::BodyStandAnimationState(const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::Stand))
    , animation_data_manager_(animation_data_manager)
{
}

std::optional<std::shared_ptr<AnimationState>> BodyStandAnimationState::HandleInput(
  Soldier& soldier)
{
    if (soldier.legs_animation_state_machine->GetType() == AnimationType::Roll) {
        return std::make_shared<BodyRollAnimationState>(animation_data_manager_);
    }

    if (soldier.legs_animation_state_machine->GetType() == AnimationType::RollBack) {
        return std::make_shared<BodyRollBackAnimationState>(animation_data_manager_);
    }

    if (soldier.control.change) {
        return std::make_shared<BodyChangeAnimationState>(animation_data_manager_);
    }

    if (soldier.control.drop &&
        soldier.weapons[soldier.active_weapon].GetWeaponParameters().kind != WeaponType::NoWeapon) {
        return std::make_shared<BodyThrowWeaponAnimationState>(animation_data_manager_);
    }

    auto maybe_throw_grenade_animation_state =
      CommonAnimationStateTransitions::TryTransitionToThrowingGrenade(soldier,
                                                                      animation_data_manager_);
    if (maybe_throw_grenade_animation_state.has_value()) {
        return *maybe_throw_grenade_animation_state;
    }

    if (soldier.control.fire &&
        (soldier.weapons[soldier.active_weapon].GetWeaponParameters().kind ==
           WeaponType::NoWeapon ||
         soldier.weapons[soldier.active_weapon].GetWeaponParameters().kind == WeaponType::Knife)) {
        return std::make_shared<BodyPunchAnimationState>(animation_data_manager_);
    }

    if (soldier.stance == PhysicsConstants::STANCE_CROUCH) {
        return std::make_shared<BodyAimAnimationState>(animation_data_manager_);
    }

    if (soldier.stance == PhysicsConstants::STANCE_PRONE) {
        return std::make_shared<BodyProneAnimationState>(animation_data_manager_);
    }

    return std::nullopt;
}

void BodyStandAnimationState::Update(Soldier& soldier) {}
} // namespace Soldank
