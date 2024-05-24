#include "core/animations/states/LegsGetUpAnimationState.hpp"

#include "core/animations/states/LegsJumpSideAnimationState.hpp"
#include "core/animations/states/LegsJumpAnimationState.hpp"
#include "core/animations/states/LegsStandAnimationState.hpp"
#include "core/animations/states/LegsFallAnimationState.hpp"
#include "core/animations/states/LegsRunAnimationState.hpp"

#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/physics/Constants.hpp"
#include "core/entities/Soldier.hpp"

namespace Soldank
{
LegsGetUpAnimationState::LegsGetUpAnimationState(const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::GetUp))
    , animation_data_manager_(animation_data_manager)
{
}

std::optional<std::shared_ptr<AnimationState>> LegsGetUpAnimationState::HandleInput(
  Soldier& soldier)
{
    if (GetFrame() == GetFramesCount()) {
        if (soldier.on_ground) {
            return std::make_shared<LegsStandAnimationState>(animation_data_manager_);
        }

        return std::make_shared<LegsFallAnimationState>(animation_data_manager_);
    }

    // Immediately switch from unprone to jump/sidejump, because the end of the
    // unprone animation can be seen as the "wind up" for the jump
    if (GetFrame() > 20 && soldier.on_ground) {
        if (soldier.control.up) {
            if (soldier.control.left || soldier.control.right) {
                // Set sidejump frame 1 to 4 depending on which unprone frame we're in
                auto id = GetFrame() - 20;
                auto new_state =
                  std::make_shared<LegsJumpSideAnimationState>(animation_data_manager_);
                new_state->SetFrame(id);
                return new_state;
            }

            // Set jump frame 6 to 9 depending on which unprone frame we're in
            auto id = soldier.legs_animation.GetFrame() - (23 - (9 - 1));
            auto new_state = std::make_shared<LegsJumpAnimationState>(animation_data_manager_);
            new_state->SetFrame(id);
            return new_state;
        }
    } else if (GetFrame() > 23) {
        auto maybe_running_animation_state =
          CommonAnimationStateTransitions::TryTransitionToRunning(soldier, animation_data_manager_);
        if (maybe_running_animation_state.has_value()) {
            return *maybe_running_animation_state;
        }

        if (!soldier.on_ground && soldier.control.up) {
            return std::make_shared<LegsRunAnimationState>(
              animation_data_manager_, soldier.control.left, soldier.control.right);
        }
    }
    return std::nullopt;
}

void LegsGetUpAnimationState::Update(Soldier& soldier)
{
    soldier.stance = PhysicsConstants::STANCE_STAND;
}
} // namespace Soldank
