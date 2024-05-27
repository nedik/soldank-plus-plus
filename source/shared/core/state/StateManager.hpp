#ifndef __STATE_MANAGER_HPP__
#define __STATE_MANAGER_HPP__

#include "core/state/State.hpp"
#include "core/state/Control.hpp"

#include <memory>

namespace Soldank
{
class StateManager
{
public:
    State& GetState() { return state_; }

    void ChangeSoldierControlActionState(std::uint8_t soldier_id,
                                         ControlActionType control_action_type,
                                         bool new_state);
    void ChangeSoldierMousePosition(std::uint8_t soldier_id, glm::vec2 new_mouse_position);
    void SwitchSoldierWeapon(std::uint8_t soldier_id);

    void CreateProjectile(const BulletParams& bullet_params);
    const std::vector<BulletParams>& GetBulletEmitter() const;
    void ClearBulletEmitter();

private:
    Soldier& GetSoldierRef(std::uint8_t soldier_id);

    State state_;
    std::vector<BulletParams> bullet_emitter_;
};
} // namespace Soldank

#endif
