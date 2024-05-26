#include "core/animations/states/BodyChangeAnimationState.hpp"

#include "core/animations/states/BodyAimAnimationState.hpp"
#include "core/animations/states/BodyProneAnimationState.hpp"
#include "core/animations/states/BodyRollAnimationState.hpp"
#include "core/animations/states/BodyRollBackAnimationState.hpp"
#include "core/animations/states/BodyStandAnimationState.hpp"

#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/animations/AnimationData.hpp"
#include "core/entities/Soldier.hpp"
#include "core/physics/Constants.hpp"

namespace Soldank
{
BodyChangeAnimationState::BodyChangeAnimationState(
  const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::Change))
    , animation_data_manager_(animation_data_manager)
{
}

std::optional<std::shared_ptr<AnimationState>> BodyChangeAnimationState::HandleInput(
  Soldier& soldier)
{
    if (GetFrame() == 2) {
        // TODO: play sound

        // Apparently soldat skips this frame
        SetNextFrame();
    }

    if (soldier.legs_animation_state_machine->GetType() == AnimationType::Roll) {
        return std::make_shared<BodyRollAnimationState>(animation_data_manager_);
    }

    if (soldier.legs_animation_state_machine->GetType() == AnimationType::RollBack) {
        return std::make_shared<BodyRollBackAnimationState>(animation_data_manager_);
    }

    if (GetFrame() == GetFramesCount()) {
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

void BodyChangeAnimationState::Update(Soldier& soldier)
{
    if (GetFrame() == 25) {
        // SwitchWeapon(soldier);
    }
}
} // namespace Soldank
