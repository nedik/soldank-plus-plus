#include "core/animations/states/LegsCrouchRunAnimationState.hpp"

#include "core/animations/states/LegsCrouchAnimationState.hpp"
#include "core/animations/states/LegsCrouchRunBackAnimationState.hpp"
#include "core/animations/states/LegsStandAnimationState.hpp"
#include "core/animations/states/LegsFallAnimationState.hpp"
#include "core/animations/states/LegsRunBackAnimationState.hpp"
#include "core/animations/states/LegsRunAnimationState.hpp"
#include "core/animations/states/LegsJumpAnimationState.hpp"
#include "core/animations/states/LegsProneAnimationState.hpp"

#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/entities/Soldier.hpp"
#include "core/physics/Constants.hpp"

namespace Soldank
{
LegsCrouchRunAnimationState::LegsCrouchRunAnimationState(
  const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::CrouchRun))
    , animation_data_manager_(animation_data_manager)
{
}

std::optional<std::shared_ptr<AnimationState>> LegsCrouchRunAnimationState::HandleInput(
  Soldier& soldier)
{
    if (soldier.control.prone) {
        return std::make_shared<LegsProneAnimationState>(animation_data_manager_);
    }

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

    if (soldier.control.jets && soldier.jets_count > 0) {
        if (soldier.on_ground) {
            auto maybe_rolling_animation_state =
              CommonAnimationStateTransitions::TryTransitionToRolling(soldier,
                                                                      animation_data_manager_);
            if (maybe_rolling_animation_state.has_value()) {
                return *maybe_rolling_animation_state;
            }
        }

        return std::make_shared<LegsFallAnimationState>(animation_data_manager_);
    }

    if (!soldier.control.left && !soldier.control.right) {
        return std::make_shared<LegsCrouchAnimationState>(animation_data_manager_);
    }

    if (soldier.control.right && soldier.direction == -1) {
        return std::make_shared<LegsCrouchRunBackAnimationState>(animation_data_manager_);
    }

    if (soldier.control.left && soldier.direction == 1) {
        return std::make_shared<LegsCrouchRunBackAnimationState>(animation_data_manager_);
    }

    return std::nullopt;
}

void LegsCrouchRunAnimationState::Update(Soldier& soldier, const PhysicsEvents& physics_events)
{
    soldier.stance = PhysicsConstants::STANCE_CROUCH;

    if (GetSpeed() > 2) {
        soldier.particle.velocity_.x /= (float)GetSpeed();
        soldier.particle.velocity_.y /= (float)GetSpeed();
    }

    if (soldier.on_ground) {
        if (soldier.control.right && soldier.direction == 1) {
            glm::vec2 particle_force = soldier.particle.GetForce();
            soldier.particle.SetForce({ PhysicsConstants::CROUCHRUNSPEED, particle_force.y });
        } else if (soldier.control.left && soldier.direction == -1) {
            glm::vec2 particle_force = soldier.particle.GetForce();
            soldier.particle.SetForce({ -PhysicsConstants::CROUCHRUNSPEED, particle_force.y });
        }
    }
}
} // namespace Soldank
