#include "core/state/StateManager.hpp"
#include "core/state/Control.hpp"

#include "spdlog/spdlog.h"

namespace Soldank
{
void StateManager::ChangeSoldierControlActionState(std::uint8_t soldier_id,
                                                   ControlActionType control_action_type,
                                                   bool new_state)
{
    Soldier& soldier = GetSoldierRef(soldier_id);
    auto get_action_field = [](Control& soldier_control,
                               ControlActionType control_action_type) -> bool& {
        switch (control_action_type) {
            case ControlActionType::MoveLeft:
                return soldier_control.left;
            case ControlActionType::MoveRight:
                return soldier_control.right;
            case ControlActionType::Jump:
                return soldier_control.up;
            case ControlActionType::Crouch:
                return soldier_control.down;
            case ControlActionType::Fire:
                return soldier_control.fire;
            case ControlActionType::UseJets:
                return soldier_control.jets;
            case ControlActionType::ChangeWeapon:
                return soldier_control.change;
            case ControlActionType::ThrowGrenade:
                return soldier_control.throw_grenade;
            case ControlActionType::DropWeapon:
                return soldier_control.drop;
            case ControlActionType::Reload:
                return soldier_control.reload;
            case ControlActionType::Prone:
                return soldier_control.prone;
            case ControlActionType::ThrowFlag:
                return soldier_control.flag_throw;
        }
    };
    bool& target_field_to_change = get_action_field(soldier.control, control_action_type);
    target_field_to_change = new_state;
}

void StateManager::SoldierControlApply(
  std::uint8_t soldier_id,
  const std::function<void(const Soldier& soldier, Control& control)>& apply_function)
{
    Soldier& soldier = GetSoldierRef(soldier_id);
    apply_function(soldier, soldier.control);
}

void StateManager::ChangeSoldierMousePosition(std::uint8_t soldier_id,
                                              glm::vec2 new_mouse_position,
                                              bool is_camera_smooth)
{
    Soldier& soldier = GetSoldierRef(soldier_id);
    soldier.game_width = 640.0;
    soldier.game_height = 480.0;
    soldier.camera_prev = soldier.camera;

    soldier.mouse.x = new_mouse_position.x;
    soldier.mouse.y = 480.0F - new_mouse_position.y; // TODO: soldier.control.mouse_aim_y expects
                                                     // top to be 0 and bottom to be game_height

    if (is_camera_smooth) {
        auto z = 1.0F;
        glm::vec2 m{ 0.0F, 0.0F };

        m.x = z * (soldier.mouse.x - soldier.game_width / 2.0F) / 7.0F *
              ((2.0F * 640.0F / soldier.game_width - 1.0F) +
               (soldier.game_width - 640.0F) / soldier.game_width * 0.0F / 6.8F);
        m.y = z * (soldier.mouse.y - soldier.game_height / 2.0F) / 7.0F;

        glm::vec2 cam_v = soldier.camera;
        glm::vec2 p = { soldier.particle.position.x, soldier.particle.position.y };
        glm::vec2 norm = p - cam_v;
        glm::vec2 s = norm * 0.14F;
        cam_v += s;
        cam_v += m;
        soldier.camera = cam_v;

    } else {
        soldier.camera.x =
          soldier.particle.position.x + (float)(soldier.mouse.x - (soldier.game_width / 2));
        soldier.camera.y = soldier.particle.position.y -
                           (float)((480.0F - soldier.mouse.y) - (soldier.game_height / 2));
    }
}

void StateManager::SwitchSoldierWeapon(std::uint8_t soldier_id)
{
    Soldier& soldier = GetSoldierRef(soldier_id);
    int new_active_weapon = (soldier.active_weapon + 1) % 2;
    soldier.active_weapon = new_active_weapon;
    // weapons[new_active_weapon].start_up_time_count =
    //   weapons[new_active_weapon].GetWeaponParameters().start_up_time;
    soldier.weapons[new_active_weapon].ResetStartUpTimeCount();
    // weapons[new_active_weapon].reload_time_prev = weapons[new_active_weapon].reload_time_count;
    soldier.weapons[new_active_weapon].SetReloadTimePrev(
      soldier.weapons[new_active_weapon].GetReloadTimeCount());
}

void StateManager::CreateProjectile(const BulletParams& bullet_params)
{
    bullet_emitter_.push_back(bullet_params);
}

const std::vector<BulletParams>& StateManager::GetBulletEmitter() const
{
    return bullet_emitter_;
}

void StateManager::ClearBulletEmitter()
{
    bullet_emitter_.clear();
}

Soldier& StateManager::GetSoldierRef(std::uint8_t soldier_id)
{
    for (auto& soldier : state_.soldiers) {
        if (soldier.id == soldier_id) {
            return soldier;
        }
    }

    spdlog::critical("Trying to access soldier of invalid id: {}", soldier_id);
    std::unreachable();
}
} // namespace Soldank
