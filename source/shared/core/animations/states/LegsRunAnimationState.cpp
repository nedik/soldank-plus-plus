#include "core/animations/states/LegsRunAnimationState.hpp"

#include "core/animations/states/LegsStandAnimationState.hpp"
#include "core/animations/states/LegsRunBackAnimationState.hpp"
#include "core/animations/states/LegsFallAnimationState.hpp"
#include "core/animations/states/LegsJumpSideAnimationState.hpp"
#include "core/animations/states/LegsJumpAnimationState.hpp"
#include "core/animations/states/LegsProneAnimationState.hpp"

#include "core/entities/Soldier.hpp"

#include "core/physics/Constants.hpp"

namespace Soldank
{
LegsRunAnimationState::LegsRunAnimationState(const AnimationDataManager& animation_data_manager,
                                             bool was_holding_left,
                                             bool was_holding_right)
    : AnimationState(animation_data_manager.Get(AnimationType::Run))
    , animation_data_manager_(animation_data_manager)
    , was_holding_left_(was_holding_left)
    , was_holding_right_(was_holding_right)
{
}

std::optional<std::shared_ptr<AnimationState>> LegsRunAnimationState::HandleInput(Soldier& soldier)
{
    if (soldier.control.prone) {
        return std::make_shared<LegsProneAnimationState>(animation_data_manager_);
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
        return std::make_shared<LegsJumpSideAnimationState>(animation_data_manager_);
    }

    if (!was_holding_left_ || !soldier.control.right) {
        if (soldier.control.left && soldier.direction == 1) {
            return std::make_shared<LegsRunBackAnimationState>(
              animation_data_manager_, soldier.control.left, soldier.control.right);
        }
    }

    if (!was_holding_right_ || !soldier.control.left) {
        if (soldier.control.right && soldier.direction == -1) {
            return std::make_shared<LegsRunBackAnimationState>(
              animation_data_manager_, soldier.control.left, soldier.control.right);
        }
    }

    was_holding_right_ = soldier.control.right;
    was_holding_left_ = soldier.control.left;
    return std::nullopt;
}

void LegsRunAnimationState::Update(Soldier& soldier)
{
    soldier.stance = 1;
    if (soldier.control.left && !soldier.control.up && soldier.direction == -1) {
        glm::vec2 particle_force = soldier.particle.GetForce();
        if (soldier.on_ground) {
            particle_force.x = -PhysicsConstants::RUNSPEED;
            particle_force.y = -PhysicsConstants::RUNSPEEDUP;
        } else {
            particle_force.x = -PhysicsConstants::FLYSPEED;
        }
        soldier.particle.SetForce(particle_force);
    } else if (soldier.control.right && !soldier.control.up && soldier.direction == 1) {
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
} // namespace Soldank
