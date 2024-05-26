#include "core/animations/states/BodyAimAnimationState.hpp"

#include "core/animations/states/BodyChangeAnimationState.hpp"
#include "core/animations/states/BodyStandAnimationState.hpp"
#include "core/animations/states/BodyProneAnimationState.hpp"
#include "core/animations/states/BodyThrowWeaponAnimationState.hpp"

#include "core/animations/states/CommonAnimationStateTransitions.hpp"

#include "core/animations/AnimationData.hpp"
#include "core/entities/Soldier.hpp"
#include "core/physics/Constants.hpp"

namespace Soldank
{
BodyAimAnimationState::BodyAimAnimationState(const AnimationDataManager& animation_data_manager)
    : AnimationState(animation_data_manager.Get(AnimationType::Aim))
    , animation_data_manager_(animation_data_manager)
{
}

std::optional<std::shared_ptr<AnimationState>> BodyAimAnimationState::HandleInput(Soldier& soldier)
{
    if (soldier.control.change) {
        return std::make_shared<BodyChangeAnimationState>(animation_data_manager_);
    }

    if (soldier.control.drop &&
        soldier.weapons[0].GetWeaponParameters().kind != WeaponType::NoWeapon) {
        return std::make_shared<BodyThrowWeaponAnimationState>(animation_data_manager_);
    }

    auto maybe_throw_grenade_animation_state =
      CommonAnimationStateTransitions::TryTransitionToThrowingGrenade(soldier,
                                                                      animation_data_manager_);
    if (maybe_throw_grenade_animation_state.has_value()) {
        return *maybe_throw_grenade_animation_state;
    }

    if (soldier.stance == PhysicsConstants::STANCE_STAND) {
        return std::make_shared<BodyStandAnimationState>(animation_data_manager_);
    }

    if (soldier.stance == PhysicsConstants::STANCE_PRONE) {
        return std::make_shared<BodyProneAnimationState>(animation_data_manager_);
    }

    return std::nullopt;
}

void BodyAimAnimationState::Update(Soldier& soldier) {}
} // namespace Soldank
