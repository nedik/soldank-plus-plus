#include "core/animations/states/BodyRollAnimationState.hpp"

#include "core/animations/states/BodyProneAnimationState.hpp"
#include "core/animations/states/BodyAimAnimationState.hpp"
#include "core/animations/states/BodyStandAnimationState.hpp"

#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/animations/AnimationData.hpp"
#include "core/entities/Soldier.hpp"
#include "core/physics/Constants.hpp"

namespace Soldank
{
BodyRollAnimationState::BodyRollAnimationState(const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::Roll))
    , animation_data_manager_(animation_data_manager)
{
}

std::optional<std::shared_ptr<AnimationState>> BodyRollAnimationState::HandleInput(Soldier& soldier)
{
    if (soldier.control.prone) {
        return std::make_shared<BodyProneAnimationState>(animation_data_manager_);
    }

    if (GetFrame() == GetFramesCount()) {
        if (soldier.stance == PhysicsConstants::STANCE_STAND) {
            return std::make_shared<BodyStandAnimationState>(animation_data_manager_);
        }

        if (soldier.stance == PhysicsConstants::STANCE_CROUCH) {
            return std::make_shared<BodyAimAnimationState>(animation_data_manager_);
        }

        if (soldier.stance == PhysicsConstants::STANCE_PRONE) {
            return std::make_shared<BodyProneAnimationState>(animation_data_manager_);
        }
    }

    return std::nullopt;
}

void BodyRollAnimationState::Update(Soldier& soldier) {}
} // namespace Soldank
