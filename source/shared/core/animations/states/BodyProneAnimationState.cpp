#include "core/animations/states/BodyProneAnimationState.hpp"

#include "core/animations/states/BodyGetUpAnimationState.hpp"
#include "core/animations/states/BodyProneMoveAnimationState.hpp"
#include "core/animations/states/BodyRollAnimationState.hpp"
#include "core/animations/states/BodyRollBackAnimationState.hpp"

#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/animations/AnimationData.hpp"
#include "core/entities/Soldier.hpp"
#include "core/physics/Constants.hpp"

namespace Soldank
{
BodyProneAnimationState::BodyProneAnimationState(const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::Prone))
    , animation_data_manager_(animation_data_manager)
{
}

std::optional<std::shared_ptr<AnimationState>> BodyProneAnimationState::HandleInput(
  Soldier& soldier)
{
    if (soldier.stance == PhysicsConstants::STANCE_STAND) {
        auto new_state = std::make_shared<BodyGetUpAnimationState>(animation_data_manager_);
        new_state->SetFrame(9);
        return new_state;
    }

    if (GetFrame() >= 23 && soldier.on_ground) {
        if (soldier.control.down && soldier.control.left && soldier.direction == -1) {
            return std::make_shared<BodyRollAnimationState>(animation_data_manager_);
        }

        if (soldier.control.down && soldier.control.right && soldier.direction == 1) {
            return std::make_shared<BodyRollAnimationState>(animation_data_manager_);
        }

        if (soldier.control.down && soldier.control.left && soldier.direction == 1) {
            return std::make_shared<BodyRollBackAnimationState>(animation_data_manager_);
        }

        if (soldier.control.down && soldier.control.right && soldier.direction == -1) {
            return std::make_shared<BodyRollBackAnimationState>(animation_data_manager_);
        }
    }

    if (soldier.legs_animation.GetFrame() > 25 && soldier.on_ground) {
        if (soldier.control.left || soldier.control.right) {
            return std::make_shared<BodyProneMoveAnimationState>(animation_data_manager_);
        }
    }

    return std::nullopt;
}

void BodyProneAnimationState::Update(Soldier& soldier) {}
} // namespace Soldank
