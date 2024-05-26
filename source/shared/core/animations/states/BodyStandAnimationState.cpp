#include "core/animations/states/BodyStandAnimationState.hpp"

#include "core/animations/states/BodyAimAnimationState.hpp"
#include "core/animations/states/BodyProneAnimationState.hpp"
#include "core/animations/states/BodyRollAnimationState.hpp"
#include "core/animations/states/BodyRollBackAnimationState.hpp"

#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/animations/AnimationData.hpp"
#include "core/entities/Soldier.hpp"
#include "core/physics/Constants.hpp"

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
