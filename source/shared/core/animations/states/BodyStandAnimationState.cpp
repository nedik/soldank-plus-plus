#include "core/animations/states/BodyStandAnimationState.hpp"

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
    return std::nullopt;
}

void BodyStandAnimationState::Update(Soldier& soldier) {}
} // namespace Soldank
