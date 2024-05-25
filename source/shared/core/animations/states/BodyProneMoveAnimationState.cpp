#include "core/animations/states/BodyProneMoveAnimationState.hpp"

#include "core/animations/states/BodyGetUpAnimationState.hpp"
#include "core/animations/states/BodyProneAnimationState.hpp"

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
    if (!soldier.control.left && !soldier.control.right) {
        auto new_state = std::make_shared<BodyProneAnimationState>(animation_data_manager_);
        new_state->SetFrame(26);
        return new_state;
    }

    if (soldier.control.prone || soldier.direction != soldier.old_direction) {
        auto new_state = std::make_shared<BodyGetUpAnimationState>(animation_data_manager_);
        new_state->SetFrame(9);
        return new_state;
    }

    if (GetFrame() == GetFramesCount()) {
        auto new_state = std::make_shared<BodyProneAnimationState>(animation_data_manager_);
        new_state->SetFrame(26);
        return new_state;
    }

    return std::nullopt;
}

void BodyProneMoveAnimationState::Update(Soldier& soldier) {}
} // namespace Soldank