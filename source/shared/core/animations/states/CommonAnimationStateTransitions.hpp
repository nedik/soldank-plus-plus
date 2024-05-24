#ifndef __COMMON_ANIMATION_STATE_TRANSITIONS_HPP__
#define __COMMON_ANIMATION_STATE_TRANSITIONS_HPP__

#include "core/animations/AnimationState.hpp"

#include <memory>

namespace Soldank
{
struct Soldier;
} // namespace Soldank
namespace Soldank::CommonAnimationStateTransitions
{
std::optional<std::shared_ptr<AnimationState>> TryTransitionToRunning(
  const Soldier& soldier,
  const AnimationDataManager& animation_data_manager);
}

#endif
