#include "core/state/StateManager.hpp"
#include "core/physics/Particles.hpp"
#include "core/state/Control.hpp"

#include "spdlog/spdlog.h"
#include <algorithm>

namespace Soldank
{
const int SECOND = 60;
const int GUN_RADIUS = 10;
const int BOW_RADIUS = 20;
const int KIT_RADIUS = 12;
const int STAT_RADIUS = 15;
const int FLAG_TIMEOUT = SECOND * 25;
// TODO: why the duplication?
const int WAYPOINT_TIMEOUT_SMALL = SECOND * 5 + 20; // = 320
const int WAYPOINT_TIMEOUT_BIG = SECOND * 8;        // = 480

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

void StateManager::CreateItem(glm::vec2 position, std::uint8_t owner_id, ItemType style)
{
    std::vector<std::uint8_t> current_ids;
    std::uint8_t new_id = 0;
    current_ids.reserve(state_.items.size());
    for (const auto& item : state_.items) {
        current_ids.push_back(item.id);
    }
    std::sort(current_ids.begin(), current_ids.end());
    while (new_id < current_ids.size() && new_id == current_ids.at(new_id)) {
        new_id++;
    }
    Item new_item;
    new_item.active = true;
    new_item.style = style;
    new_item.id = new_id;
    new_item.holding_sprite = 0;
    new_item.owner = owner_id;
    new_item.time_out = 0;
    // new_item.skeleton = std::make_shared<ParticleSystem>();
    new_item.static_type = false;
    new_item.in_base = false;

    for (std::uint8_t& i : new_item.collide_count) {
        i = 0;
    }

    if (owner_id != 255) {
        // TODO: set direction
    }

    switch (style) {
        case ItemType::AlphaFlag:
        case ItemType::BravoFlag:
        case ItemType::PointmatchFlag:
            new_item.radius = 19;
            break;
        case ItemType::DesertEagles:
        case ItemType::MP5:
        case ItemType::Ak74:
        case ItemType::SteyrAUG:
        case ItemType::Spas12:
        case ItemType::Ruger77:
        case ItemType::M79:
        case ItemType::Barrett:
        case ItemType::Minimi:
        case ItemType::Minigun:
        case ItemType::USSOCOM:
        case ItemType::Knife:
        case ItemType::Chainsaw:
        case ItemType::LAW:
        case ItemType::Bow:
            new_item.skeleton = ParticleSystem::Load(
              ParticleSystemType::Weapon); // new_item.skeleton->VDamping = 0.989;
            // new_item.skeleton->GravityMultiplier = 1.07;
            new_item.radius = KIT_RADIUS;
            new_item.time_out = FLAG_TIMEOUT;
            // new_item.interest : = DEFAULT_INTEREST_TIME;
            new_item.collide_with_bullets = true; // TODO: sv_kits_collide.Value;
            break;
        case ItemType::FlamerKit:
        case ItemType::PredatorKit:
        case ItemType::BerserkKit:
        case ItemType::MedicalKit:
        case ItemType::ClusterKit:
        case ItemType::VestKit:
        case ItemType::GrenadeKit:
            new_item.skeleton = ParticleSystem::Load(ParticleSystemType::Kit);
            // new_item.skeleton->VDamping = 0.989;
            // new_item.skeleton->GravityMultiplier = 1.07;
            new_item.radius = KIT_RADIUS;
            new_item.time_out = FLAG_TIMEOUT;
            // new_item.interest : = DEFAULT_INTEREST_TIME;
            new_item.collide_with_bullets = true; // TODO: sv_kits_collide.Value;
            break;
        case ItemType::Parachute:
        case ItemType::M2:
            break;
    }

    state_.items.push_back(new_item);
    SetItemPosition(state_.items.size() - 1, position);
}

void StateManager::SetItemPosition(unsigned int id, glm::vec2 new_position)
{
    Item& item = state_.items.at(id);
    glm::vec2 direction = new_position - item.skeleton->GetPos(1);
    MoveItemIntoDirection(id, direction);
}

void StateManager::MoveItemIntoDirection(unsigned int id, glm::vec2 direction)
{
    Item& item = state_.items.at(id);
    for (unsigned int i = 1; i <= item.skeleton->GetParticles().size(); ++i) {
        glm::vec2 new_position = item.skeleton->GetPos(i) + direction;
        item.skeleton->SetPos(i, new_position);
        item.skeleton->SetOldPos(i, new_position);
    }
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
