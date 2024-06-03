#include "core/animations/states/LegsRunBackAnimationState.hpp"

#include "core/animations/states/LegsStandAnimationState.hpp"
#include "core/animations/states/LegsRunAnimationState.hpp"
#include "core/animations/states/LegsFallAnimationState.hpp"
#include "core/animations/states/LegsJumpSideAnimationState.hpp"
#include "core/animations/states/LegsJumpAnimationState.hpp"
#include "core/animations/states/LegsProneAnimationState.hpp"

#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/physics/Constants.hpp"
#include "core/entities/Soldier.hpp"

namespace Soldank
{
LegsRunBackAnimationState::LegsRunBackAnimationState(
  const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::RunBack))
    , animation_data_manager_(animation_data_manager)
{
}

void LegsRunBackAnimationState::Enter(Soldier& soldier)
{
    was_holding_left_ = soldier.control.left;
    was_holding_right_ = soldier.control.right;
}

std::optional<std::shared_ptr<AnimationState>> LegsRunBackAnimationState::HandleInput(
  Soldier& soldier)
{
    if (soldier.control.prone) {
        return std::make_shared<LegsProneAnimationState>(animation_data_manager_);
    }

    if (soldier.on_ground) {
        bool was_holding_left = soldier.control.left;
        bool was_holding_right = soldier.control.right;
        if (IsRunningLeft(soldier)) {
            soldier.control.right = false;
        } else {
            soldier.control.left = false;
        }
        auto maybe_rolling_animation_state =
          CommonAnimationStateTransitions::TryTransitionToRolling(soldier, animation_data_manager_);
        soldier.control.left = was_holding_left;
        soldier.control.right = was_holding_right;
        if (maybe_rolling_animation_state.has_value()) {
            return *maybe_rolling_animation_state;
        }
    }

    if (!soldier.control.left && !soldier.control.right) {
        if (soldier.on_ground) {
            if (soldier.control.up) {
                return std::make_shared<LegsJumpAnimationState>(animation_data_manager_);
            }
            return std::make_shared<LegsStandAnimationState>(animation_data_manager_);
        }

        if (soldier.control.up) {
            was_holding_right_ = soldier.control.right;
            was_holding_left_ = soldier.control.left;
            return std::nullopt;
        }

        return std::make_shared<LegsFallAnimationState>(animation_data_manager_);
    }

    if (soldier.control.up && soldier.on_ground) {
        soldier.control.was_running_left = IsRunningLeft(soldier);
        return std::make_shared<LegsJumpSideAnimationState>(animation_data_manager_);
    }

    if (!was_holding_left_ || !soldier.control.right) {
        if (soldier.control.left && soldier.direction == -1) {
            return std::make_shared<LegsRunAnimationState>(animation_data_manager_);
        }
    }

    if (!was_holding_right_ || !soldier.control.left) {
        if (soldier.control.right && soldier.direction == 1) {
            return std::make_shared<LegsRunAnimationState>(animation_data_manager_);
        }
    }

    // if using jets, reset animation because first frame looks like "directional" jetting
    if (soldier.control.jets && soldier.jets_count > 0) {
        return std::make_shared<LegsRunBackAnimationState>(animation_data_manager_);
    }

    was_holding_right_ = soldier.control.right;
    was_holding_left_ = soldier.control.left;
    return std::nullopt;
}

void LegsRunBackAnimationState::Update(Soldier& soldier, const PhysicsEvents& physics_events)
{
    soldier.stance = PhysicsConstants::STANCE_STAND;

    if (soldier.control.left && !soldier.control.up && soldier.direction == 1) {
        glm::vec2 particle_force = soldier.particle.GetForce();
        if (soldier.on_ground) {
            particle_force.x = -PhysicsConstants::RUNSPEED;
            particle_force.y = -PhysicsConstants::RUNSPEEDUP;
        } else {
            particle_force.x = -PhysicsConstants::FLYSPEED;
        }
        soldier.particle.SetForce(particle_force);
    } else if (soldier.control.right && !soldier.control.up && soldier.direction == -1) {
        if (soldier.on_ground) {
            glm::vec2 particle_force = soldier.particle.GetForce();
            particle_force.x = PhysicsConstants::RUNSPEED;
            particle_force.y = -PhysicsConstants::RUNSPEEDUP;
            soldier.particle.SetForce(particle_force);
        } else {
            glm::vec2 particle_force = soldier.particle.GetForce();
            particle_force.x = PhysicsConstants::FLYSPEED;
            soldier.particle.SetForce(particle_force);
        }
    }
}

bool LegsRunBackAnimationState::IsRunningLeft(const Soldier& soldier) const
{
    if (soldier.control.left && soldier.control.right) {
        if (soldier.direction == 1) {
            if (!was_holding_right_) {
                // right was just pressed so should be running right
                return false;
            }
        }

        if (soldier.direction == -1) {
            if (!was_holding_left_) {
                // left was just pressed so should be running left
                return true;
            }
        }

        return soldier.direction == 1;
    }

    return soldier.control.left;
}
} // namespace Soldank
