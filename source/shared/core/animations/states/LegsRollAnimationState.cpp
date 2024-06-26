#include "core/animations/states/LegsRollAnimationState.hpp"

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
LegsRollAnimationState::LegsRollAnimationState(const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::Roll))
    , animation_data_manager_(animation_data_manager)
{
}

void LegsRollAnimationState::Enter(Soldier& soldier)
{
    if (soldier.on_ground) {
        glm::vec2 particle_force = soldier.particle.GetForce();
        soldier.particle.SetForce(
          { (float)soldier.direction * 2.0F * PhysicsConstants::CROUCHRUNSPEED, particle_force.y });
    }
}

std::optional<std::shared_ptr<AnimationState>> LegsRollAnimationState::HandleInput(Soldier& soldier)
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

void LegsRollAnimationState::Update(Soldier& soldier, const PhysicsEvents& physics_events)
{
    soldier.stance = PhysicsConstants::STANCE_STAND;

    if (GetSpeed() > 1) {
        soldier.particle.velocity_.x /= (float)GetSpeed();
        soldier.particle.velocity_.y /= (float)GetSpeed();
    }

    if (soldier.on_ground) {
        glm::vec2 particle_force = soldier.particle.GetForce();
        soldier.particle.SetForce(
          { (float)soldier.direction * PhysicsConstants::ROLLSPEED, particle_force.y });
    } else {
        glm::vec2 particle_force = soldier.particle.GetForce();
        soldier.particle.SetForce(
          { (float)soldier.direction * 2.0F * PhysicsConstants::FLYSPEED, particle_force.y });
    }
}
} // namespace Soldank
