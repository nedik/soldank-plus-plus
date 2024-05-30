#include "core/animations/states/LegsRollBackAnimationState.hpp"

#include "core/animations/states/LegsCrouchAnimationState.hpp"
#include "core/animations/states/LegsGetUpAnimationState.hpp"
#include "core/animations/states/LegsProneAnimationState.hpp"
#include "core/animations/states/LegsJumpAnimationState.hpp"
#include "core/animations/states/LegsStandAnimationState.hpp"
#include "core/animations/states/LegsFallAnimationState.hpp"

#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/entities/Soldier.hpp"
#include "core/physics/Constants.hpp"

namespace Soldank
{
LegsRollBackAnimationState::LegsRollBackAnimationState(
  const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::RollBack))
    , animation_data_manager_(animation_data_manager)
{
}

void LegsRollBackAnimationState::Enter(Soldier& soldier)
{
    if (soldier.on_ground) {
        glm::vec2 particle_force = soldier.particle.GetForce();
        soldier.particle.SetForce(
          { -(float)soldier.direction * 2.0F * PhysicsConstants::CROUCHRUNSPEED,
            particle_force.y });
    }
}

std::optional<std::shared_ptr<AnimationState>> LegsRollBackAnimationState::HandleInput(
  Soldier& soldier)
{
    if (soldier.control.prone) {
        return std::make_shared<LegsProneAnimationState>(animation_data_manager_);
    }

    if (GetFrame() == GetFramesCount()) {
        auto maybe_crouch_run_animation_state =
          CommonAnimationStateTransitions::TryTransitionToCrouchRunning(soldier,
                                                                        animation_data_manager_);
        if (maybe_crouch_run_animation_state.has_value()) {
            return *maybe_crouch_run_animation_state;
        }

        if (soldier.control.down) {
            return std::make_shared<LegsCrouchAnimationState>(animation_data_manager_);
        }

        auto maybe_running_animation_state =
          CommonAnimationStateTransitions::TryTransitionToRunning(soldier, animation_data_manager_);
        if (maybe_running_animation_state.has_value()) {
            return *maybe_running_animation_state;
        }

        if (soldier.on_ground) {
            if (soldier.control.up) {
                return std::make_shared<LegsJumpAnimationState>(animation_data_manager_);
            }

            if (soldier.control.down) {
                return std::make_shared<LegsCrouchAnimationState>(animation_data_manager_);
            }

            return std::make_shared<LegsStandAnimationState>(animation_data_manager_);
        }

        return std::make_shared<LegsFallAnimationState>(animation_data_manager_);
    }

    return std::nullopt;
}

void LegsRollBackAnimationState::Update(Soldier& soldier, const PhysicsEvents& physics_events)
{
    soldier.stance = PhysicsConstants::STANCE_STAND;

    if (GetSpeed() > 1) {
        soldier.particle.velocity_.x /= (float)GetSpeed();
        soldier.particle.velocity_.y /= (float)GetSpeed();
    }

    if (soldier.on_ground) {
        glm::vec2 particle_force = soldier.particle.GetForce();
        soldier.particle.SetForce(
          { -(float)soldier.direction * PhysicsConstants::ROLLSPEED, particle_force.y });
    } else {
        glm::vec2 particle_force = soldier.particle.GetForce();
        soldier.particle.SetForce(
          { -(float)soldier.direction * 2.0F * PhysicsConstants::FLYSPEED, particle_force.y });
    }

    if ((GetFrame() > 1) && (GetFrame() < 8)) {
        // so apparently in soldat there's no difference between a RollBack and a backflip
        // if you press W during a Rollback in those frames then it pushes you up and therefore
        // it turns into a backflip. That's how also cannonballs works, if we release W fast enough,
        // it will turn into a RollBack
        if (soldier.control.up) {
            glm::vec2 particle_force = soldier.particle.GetForce();
            particle_force.y -= PhysicsConstants::JUMPDIRSPEED * 1.5F;
            particle_force.x *= 0.5;
            soldier.particle.SetForce(particle_force);
            soldier.particle.velocity_.x *= 0.8;
        }
    }
}
} // namespace Soldank
