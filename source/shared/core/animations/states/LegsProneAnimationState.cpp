#include "core/animations/states/LegsProneAnimationState.hpp"

#include "core/animations/states/LegsCrouchRunAnimationState.hpp"
#include "core/animations/states/LegsStandAnimationState.hpp"
#include "core/animations/states/LegsFallAnimationState.hpp"
#include "core/animations/states/LegsRunBackAnimationState.hpp"
#include "core/animations/states/LegsRunAnimationState.hpp"
#include "core/animations/states/LegsJumpAnimationState.hpp"

#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/entities/Soldier.hpp"
#include "core/physics/Constants.hpp"

namespace Soldank
{
LegsProneAnimationState::LegsProneAnimationState(const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::Prone))
    , animation_data_manager_(animation_data_manager)
{
}

std::optional<std::shared_ptr<AnimationState>> LegsProneAnimationState::HandleInput(
  Soldier& soldier)
{
    return std::nullopt;
}

void LegsProneAnimationState::Update(Soldier& soldier)
{
    soldier.stance = PhysicsConstants::STANCE_PRONE;
}
} // namespace Soldank
