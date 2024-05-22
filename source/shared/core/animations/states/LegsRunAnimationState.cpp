#include "core/animations/states/LegsRunAnimationState.hpp"

#include "core/entities/Soldier.hpp"
#include "core/animations/states/LegsStandAnimationState.hpp"
#include "core/animations/states/LegsRunBackAnimationState.hpp"

const float SLIDELIMIT = 0.2F;
const float GRAV = 0.06F;
const float SURFACECOEFX = 0.970F;
const float SURFACECOEFY = 0.970F;
const float CROUCHMOVESURFACECOEFX = 0.85F;
const float CROUCHMOVESURFACECOEFY = 0.97F;
const float STANDSURFACECOEFX = 0.00F;
const float STANDSURFACECOEFY = 0.00F;

const std::uint8_t STANCE_STAND = 1;
const std::uint8_t STANCE_CROUCH = 2;
const std::uint8_t STANCE_PRONE = 3;

const float MAX_VELOCITY = 11.0F;
const float SOLDIER_COL_RADIUS = 3.0F;

const float RUNSPEED = 0.118F;
const float RUNSPEEDUP = RUNSPEED / 6.0F;
const float FLYSPEED = 0.03F;
const float JUMPSPEED = 0.66F;
const float CROUCHRUNSPEED = RUNSPEED / 0.6F;
const float PRONESPEED = RUNSPEED * 4.0F;
const float ROLLSPEED = RUNSPEED / 1.2F;
const float JUMPDIRSPEED = 0.30F;
const float JETSPEED = 0.10F;
const int SECOND = 60;

const int DEFAULT_IDLETIME = SECOND * 8;

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

void LegsRunAnimationState::HandleInput(Soldier& soldier)
{
    if (!soldier.control.left && !soldier.control.right) {
        soldier.legs_animation_state_machine =
          std::make_unique<LegsStandAnimationState>(animation_data_manager_);
        return;
    }

    if (!was_holding_left_ || !soldier.control.right) {
        if (soldier.control.left && soldier.direction == 1) {
            soldier.legs_animation_state_machine = std::make_unique<LegsRunBackAnimationState>(
              animation_data_manager_, soldier.control.left, soldier.control.right);
            return;
        }
    }

    if (!was_holding_right_ || !soldier.control.left) {
        if (soldier.control.right && soldier.direction == -1) {
            soldier.legs_animation_state_machine = std::make_unique<LegsRunBackAnimationState>(
              animation_data_manager_, soldier.control.left, soldier.control.right);
            return;
        }
    }

    was_holding_right_ = soldier.control.right;
    was_holding_left_ = soldier.control.left;
}

void LegsRunAnimationState::Update(Soldier& soldier)
{
    soldier.stance = 1;
    if (soldier.direction == -1) {
        glm::vec2 particle_force = soldier.particle.GetForce();
        if (soldier.on_ground) {
            particle_force.x = -RUNSPEED;
            particle_force.y = -RUNSPEEDUP;
        } else {
            particle_force.x = -FLYSPEED;
        }
        soldier.particle.SetForce(particle_force);
    } else {
        if (soldier.on_ground) {
            glm::vec2 particle_force = soldier.particle.GetForce();
            particle_force.x = RUNSPEED;
            particle_force.y = -RUNSPEEDUP;
            soldier.particle.SetForce(particle_force);
        } else {
            glm::vec2 particle_force = soldier.particle.GetForce();
            particle_force.x = FLYSPEED;
            soldier.particle.SetForce(particle_force);
        }
    }
}
} // namespace Soldank
