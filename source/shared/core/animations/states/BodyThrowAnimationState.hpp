#ifndef __BODY_THROW_ANIMATION_STATE_HPP__
#define __BODY_THROW_ANIMATION_STATE_HPP__

#include "core/animations/AnimationState.hpp"

#include "core/animations/AnimationData.hpp"

namespace Soldank
{
struct Soldier;

class BodyThrowAnimationState final : public Soldank::AnimationState
{
public:
    BodyThrowAnimationState(const AnimationDataManager& animation_data_manager);
    ~BodyThrowAnimationState() override = default;

    void Enter(Soldier& soldier) final;
    std::optional<std::shared_ptr<AnimationState>> HandleInput(Soldier& soldier) final;
    void Update(Soldier& soldier, const PhysicsEvents& physics_events) final;

private:
    bool IsSoldierShootingPossible(const Soldier& /*soldier*/) const final { return true; }

    const AnimationDataManager& animation_data_manager_;
};
} // namespace Soldank

#endif
