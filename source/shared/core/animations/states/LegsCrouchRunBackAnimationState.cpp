#include "core/animations/states/LegsCrouchRunBackAnimationState.hpp"

#include "core/animations/states/LegsCrouchAnimationState.hpp"
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
LegsCrouchRunBackAnimationState::LegsCrouchRunBackAnimationState(
  const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::CrouchRunBack))
    , animation_data_manager_(animation_data_manager)
{
}

std::optional<std::shared_ptr<AnimationState>> LegsCrouchRunBackAnimationState::HandleInput(
  Soldier& soldier)
{
    if (!soldier.control.down) {
        auto maybe_running_animation_state =
          CommonAnimationStateTransitions::TryTransitionToRunning(soldier, animation_data_manager_);
        if (maybe_running_animation_state.has_value()) {
            return *maybe_running_animation_state;
        }

        if (soldier.on_ground) {
            return std::make_shared<LegsStandAnimationState>(animation_data_manager_);
        }

        return std::make_shared<LegsFallAnimationState>(animation_data_manager_);
    }

    if (!soldier.control.left && !soldier.control.right) {
        return std::make_shared<LegsCrouchAnimationState>(animation_data_manager_);
    }

    if (soldier.control.right && soldier.direction == 1) {
        return std::make_shared<LegsCrouchRunAnimationState>(animation_data_manager_);
    }

    if (soldier.control.left && soldier.direction == -1) {
        return std::make_shared<LegsCrouchRunAnimationState>(animation_data_manager_);
    }

    return std::nullopt;
}

void LegsCrouchRunBackAnimationState::Update(Soldier& soldier)
{
    soldier.stance = PhysicsConstants::STANCE_CROUCH;

    if (soldier.on_ground) {
        if (soldier.control.right && soldier.direction == -1) {
            glm::vec2 particle_force = soldier.particle.GetForce();
            soldier.particle.SetForce({ PhysicsConstants::CROUCHRUNSPEED, particle_force.y });
        } else if (soldier.control.left && soldier.direction == 1) {
            glm::vec2 particle_force = soldier.particle.GetForce();
            soldier.particle.SetForce({ -PhysicsConstants::CROUCHRUNSPEED, particle_force.y });
        }
    }
}
} // namespace Soldank
