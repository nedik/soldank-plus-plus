#ifndef __WORLD_HPP__
#define __WORLD_HPP__

#include "core/state/State.hpp"
#include "core/physics/Soldier.hpp"
#include "core/physics/BulletPhysics.hpp"

namespace Soldat
{
class World
{
public:
    World();

    void Update(double delta_time);
    const std::shared_ptr<State>& GetState() const;
    const Soldier& GetSoldier() const;

    void UpdateFireButtonState(bool pressed);
    void UpdateJetsButtonState(bool pressed);
    void UpdateLeftButtonState(bool pressed);
    void UpdateRightButtonState(bool pressed);
    void UpdateJumpButtonState(bool pressed);
    void UpdateCrouchButtonState(bool pressed);
    void UpdateProneButtonState(bool pressed);
    void UpdateChangeButtonState(bool pressed);
    void UpdateThrowGrenadeButtonState(bool pressed);
    void UpdateDropButtonState(bool pressed);
    void UpdateMousePosition(glm::vec2 mouse_position);

private:
    std::shared_ptr<State> state_;
    std::unique_ptr<Soldier> soldier_;
    std::unique_ptr<BulletPhysics> bullet_physics_;
};

} // namespace Soldat

#endif
