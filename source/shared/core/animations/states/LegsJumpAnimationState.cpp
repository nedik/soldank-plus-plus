#include "core/animations/states/LegsJumpAnimationState.hpp"

#include "core/animations/states/LegsStandAnimationState.hpp"
#include "core/animations/states/LegsFallAnimationState.hpp"
#include "core/animations/states/LegsProneAnimationState.hpp"

#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/physics/Constants.hpp"
#include "core/entities/Soldier.hpp"

namespace Soldank
{
LegsJumpAnimationState::LegsJumpAnimationState(const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::Jump))
    , animation_data_manager_(animation_data_manager)
{
}

std::optional<std::shared_ptr<AnimationState>> LegsJumpAnimationState::HandleInput(Soldier& soldier)
{
    if (soldier.control.prone) {
        return std::make_shared<LegsProneAnimationState>(animation_data_manager_);
    }

    if (!soldier.control.up) {
        if (soldier.on_ground) {
            return std::make_shared<LegsStandAnimationState>(animation_data_manager_);
        }

        return std::make_shared<LegsFallAnimationState>(animation_data_manager_);
    }

    if (GetFrame() == GetFramesCount()) {
        // If holding A or D, don't change animation. TODO: figure out why soldat does it and
        // improve explanation
        if (!soldier.control.left && !soldier.control.right) {
            if (soldier.on_ground) {
                return std::make_shared<LegsStandAnimationState>(animation_data_manager_);
            }

            return std::make_shared<LegsFallAnimationState>(animation_data_manager_);
        }

        if (soldier.on_ground) {
            auto maybe_running_animation_state =
              CommonAnimationStateTransitions::TryTransitionToRunning(soldier,
                                                                      animation_data_manager_);
            if (maybe_running_animation_state.has_value()) {
                return *maybe_running_animation_state;
            }
        }
    }
    return std::nullopt;
}

void LegsJumpAnimationState::Update(Soldier& soldier)
{
    soldier.stance = PhysicsConstants::STANCE_STAND;
    if ((GetFrame() > 8) && (GetFrame() < 15)) {
        glm::vec particle_force = soldier.particle.GetForce();
        particle_force.y = -PhysicsConstants::JUMPSPEED;
        soldier.particle.SetForce(particle_force);
    }
}
} // namespace Soldank
