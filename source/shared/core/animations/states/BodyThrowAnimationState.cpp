#include "core/animations/states/BodyThrowAnimationState.hpp"

#include "core/animations/states/BodyAimAnimationState.hpp"
#include "core/animations/states/BodyChangeAnimationState.hpp"
#include "core/animations/states/BodyGetUpAnimationState.hpp"
#include "core/animations/states/BodyProneAnimationState.hpp"
#include "core/animations/states/BodyRollAnimationState.hpp"
#include "core/animations/states/BodyRollBackAnimationState.hpp"
#include "core/animations/states/BodyStandAnimationState.hpp"
#include "core/animations/states/BodyThrowWeaponAnimationState.hpp"

#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/animations/AnimationData.hpp"
#include "core/entities/Soldier.hpp"
#include "core/physics/Constants.hpp"
#include <memory>

namespace Soldank
{
BodyThrowAnimationState::BodyThrowAnimationState(const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::Throw))
    , animation_data_manager_(animation_data_manager)
{
}

void BodyThrowAnimationState::Enter(Soldier& soldier)
{
    soldier.grenade_can_throw = false;
}

std::optional<std::shared_ptr<AnimationState>> BodyThrowAnimationState::HandleInput(
  Soldier& soldier)
{
    if (soldier.legs_animation->GetType() == AnimationType::Roll) {
        return std::make_shared<BodyRollAnimationState>(animation_data_manager_);
    }

    if (soldier.legs_animation->GetType() == AnimationType::RollBack) {
        return std::make_shared<BodyRollBackAnimationState>(animation_data_manager_);
    }

    // Prone cancelling
    if (soldier.legs_animation->GetType() == AnimationType::GetUp) {
        auto new_state = std::make_shared<BodyGetUpAnimationState>(animation_data_manager_);
        new_state->SetFrame(9);
        return new_state;
    }

    if (soldier.control.change) {
        return std::make_shared<BodyChangeAnimationState>(animation_data_manager_);
    }

    if (GetFrame() == GetFramesCount() || !soldier.control.throw_grenade) {
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

void BodyThrowAnimationState::Update(Soldier& soldier, const PhysicsEvents& physics_events) {}
} // namespace Soldank
