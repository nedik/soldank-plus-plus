#include "core/animations/states/LegsProneMoveAnimationState.hpp"

#include "core/animations/states/LegsGetUpAnimationState.hpp"
#include "core/animations/states/LegsProneAnimationState.hpp"

#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/entities/Soldier.hpp"
#include "core/physics/Constants.hpp"

namespace Soldank
{
LegsProneMoveAnimationState::LegsProneMoveAnimationState(
  const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::ProneMove))
    , animation_data_manager_(animation_data_manager)
{
}

void LegsProneMoveAnimationState::Enter(Soldier& soldier)
{
    moving_direction_left_ = soldier.control.left;
    both_direction_keys_pressed_ = soldier.control.left && soldier.control.right;
}

std::optional<std::shared_ptr<AnimationState>> LegsProneMoveAnimationState::HandleInput(
  Soldier& soldier)
{
    if (!soldier.control.left || !soldier.control.right) {
        both_direction_keys_pressed_ = false;
        moving_direction_left_ = soldier.control.left;
    }
    if (!both_direction_keys_pressed_ && soldier.control.left && soldier.control.right) {
        both_direction_keys_pressed_ = true;
        moving_direction_left_ = !moving_direction_left_;
    }

    if (!soldier.control.left && !soldier.control.right) {
        auto new_state = std::make_shared<LegsProneAnimationState>(animation_data_manager_);
        new_state->SetFrame(26);
        return new_state;
    }

    if (soldier.on_ground) {
        bool was_holding_left = soldier.control.left;
        bool was_holding_right = soldier.control.right;
        if (moving_direction_left_) {
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

    if (soldier.control.prone || soldier.direction != soldier.old_direction) {
        auto new_state = std::make_shared<LegsGetUpAnimationState>(animation_data_manager_);
        new_state->SetFrame(9);
        return new_state;
    }

    return std::nullopt;
}

void LegsProneMoveAnimationState::Update(Soldier& soldier, const PhysicsEvents& physics_events)
{
    soldier.stance = PhysicsConstants::STANCE_PRONE;

    if (GetSpeed() > 2) {
        soldier.particle.velocity_.x /= (float)GetSpeed();
        soldier.particle.velocity_.y /= (float)GetSpeed();
    }

    if ((GetFrame() < 4) || (GetFrame() > 14)) {
        if (soldier.on_ground) {
            if (moving_direction_left_) {
                glm::vec2 particle_force = soldier.particle.GetForce();
                particle_force.x = -PhysicsConstants::PRONESPEED;
                soldier.particle.SetForce(particle_force);
            } else {
                glm::vec2 particle_force = soldier.particle.GetForce();
                particle_force.x = PhysicsConstants::PRONESPEED;
                soldier.particle.SetForce(particle_force);
            }
        }
    }
}
} // namespace Soldank
