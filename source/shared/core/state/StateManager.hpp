#ifndef __STATE_MANAGER_HPP__
#define __STATE_MANAGER_HPP__

#include "core/state/State.hpp"
#include "core/state/Control.hpp"

#include "core/types/ItemType.hpp"

#include <memory>
#include <functional>

namespace Soldank
{
class StateManager
{
public:
    State& GetState() { return state_; }

    void ChangeSoldierControlActionState(std::uint8_t soldier_id,
                                         ControlActionType control_action_type,
                                         bool new_state);
    void SoldierControlApply(
      std::uint8_t soldier_id,
      const std::function<void(const Soldier& soldier, Control& control)>& apply_function);

    void ChangeSoldierMousePosition(std::uint8_t soldier_id,
                                    glm::vec2 new_mouse_position,
                                    bool is_camera_smooth);
    void SwitchSoldierWeapon(std::uint8_t soldier_id);
    void ChangeSoldierPrimaryWeapon(std::uint8_t soldier_id, WeaponType new_weapon_type);
    void SoldierPickupWeapon(std::uint8_t soldier_id, const Item& item);
    void ThrowSoldierFlags(std::uint8_t soldier_id);
    glm::vec2 GetSoldierAimDirection(std::uint8_t soldier_id);

    void CreateProjectile(const BulletParams& bullet_params);
    const std::vector<BulletParams>& GetBulletEmitter() const;
    void ClearBulletEmitter();

    void CreateItem(glm::vec2 position, std::uint8_t owner_id, ItemType style);
    void SetItemPosition(unsigned int id, glm::vec2 new_position);
    void MoveItemIntoDirection(unsigned int id, glm::vec2 direction);

private:
    Soldier& GetSoldierRef(std::uint8_t soldier_id);

    State state_;
    std::vector<BulletParams> bullet_emitter_;
};
} // namespace Soldank

#endif
