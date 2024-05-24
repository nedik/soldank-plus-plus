#include "core/animations/states/BodyProneAnimationState.hpp"

#include "core/animations/states/BodyGetUpAnimationState.hpp"
#include "core/animations/states/BodyStandAnimationState.hpp"
#include "core/animations/states/BodyAimAnimationState.hpp"

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

    return std::nullopt;
}

void BodyProneAnimationState::Update(Soldier& soldier) {}
} // namespace Soldank
