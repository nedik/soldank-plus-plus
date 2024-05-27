#include "core/animations/states/BodyThrowWeaponAnimationState.hpp"

#include "core/animations/states/BodyAimAnimationState.hpp"
#include "core/animations/states/BodyChangeAnimationState.hpp"
#include "core/animations/states/BodyProneAnimationState.hpp"
#include "core/animations/states/BodyRollAnimationState.hpp"
#include "core/animations/states/BodyRollBackAnimationState.hpp"
#include "core/animations/states/BodyStandAnimationState.hpp"

#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/animations/AnimationData.hpp"
#include "core/entities/Soldier.hpp"
#include "core/physics/PhysicsEvents.hpp"
#include "core/physics/Constants.hpp"

#include <memory>

namespace Soldank
{
BodyThrowWeaponAnimationState::BodyThrowWeaponAnimationState(
  const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::ThrowWeapon))
    , animation_data_manager_(animation_data_manager)
{
}

void BodyThrowWeaponAnimationState::Enter(Soldier& soldier)
{
    if (soldier.weapons[soldier.active_weapon].GetWeaponParameters().kind == WeaponType::Knife) {
        SetSpeed(2);
    }
}

std::optional<std::shared_ptr<AnimationState>> BodyThrowWeaponAnimationState::HandleInput(
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

    if ((soldier.weapons[soldier.active_weapon].GetWeaponParameters().kind == WeaponType::Knife &&
         (!soldier.control.drop || soldier.body_animation.GetFrame() == 16)) ||
        GetFrame() == GetFramesCount()) {

        if (soldier.stance == PhysicsConstants::STANCE_CROUCH) {
            return std::make_shared<BodyAimAnimationState>(animation_data_manager_);
        }

        if (soldier.stance == PhysicsConstants::STANCE_PRONE) {
            auto prone_animation_state =
              std::make_shared<BodyProneAnimationState>(animation_data_manager_);
            prone_animation_state->SetFrame(26);
            return prone_animation_state;
        }

        if (soldier.stance == PhysicsConstants::STANCE_STAND) {
            return std::make_shared<BodyStandAnimationState>(animation_data_manager_);
        }
    }

    return std::nullopt;
}

void BodyThrowWeaponAnimationState::Exit(Soldier& soldier, const PhysicsEvents& physics_events)
{
    if (soldier.weapons[soldier.active_weapon].GetWeaponParameters().kind == WeaponType::Knife) {
        physics_events.soldier_throws_knife.Notify(soldier);
    }
}
} // namespace Soldank
