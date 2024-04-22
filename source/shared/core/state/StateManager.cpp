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
