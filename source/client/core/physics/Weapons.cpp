#include "Weapons.hpp"
#include "core/types/WeaponType.hpp"

#include <SimpleIni.h>

#include <algorithm>
#include <utility>
#include <iostream>

namespace Soldat
{
const std::uint16_t SECOND = 60;
const std::uint16_t BULLET_TIMEOUT = SECOND * 7;
const std::uint16_t GRENADE_TIMEOUT = SECOND * 3;
const std::uint16_t M2BULLET_TIMEOUT = SECOND;
const std::uint16_t FLAMER_TIMEOUT = SECOND * 32;
const std::uint16_t MELEE_TIMEOUT = 1;

Weapon::Weapon(WeaponType kind, bool realistic)
{
    CSimpleIniA ini_file;
    std::string ini_file_path;
    if (realistic) {
        ini_file_path = "weapons_realistic.ini";
    } else {
        ini_file_path = "weapons.ini";
    }
    SI_Error rc = ini_file.LoadFile(ini_file_path.c_str());
    if (rc < 0) {
        std::cout << "Error: INI File could not be loaded: " << ini_file_path << std::endl;
    };

    switch (kind) {
        case WeaponType::DesertEagles:
            name = "Desert Eagles";
            ini_name = "Desert Eagles";
            break;
        case WeaponType::MP5:
            name = "HK MP5";
            ini_name = "HK MP5";
            break;
        case WeaponType::Ak74:
            name = "Ak-74";
            ini_name = "Ak-74";
            break;
        case WeaponType::SteyrAUG:
            name = "Steyr AUG";
            ini_name = "Steyr AUG";
            break;
        case WeaponType::Spas12:
            name = "Spas-12";
            ini_name = "Spas-12";
            break;
        case WeaponType::Ruger77:
            name = "Ruger 77";
            ini_name = "Ruger 77";
            break;
        case WeaponType::M79:
            name = "M79";
            ini_name = "M79";
            break;
        case WeaponType::Barrett:
            name = "Barrett M82A1";
            ini_name = "Barret M82A1";
            break;
        case WeaponType::Minimi:
            name = "FN Minimi";
            ini_name = "FN Minimi";
            break;
        case WeaponType::Minigun:
            name = "XM214 Minigun";
            ini_name = "XM214 Minigun";
            break;
        case WeaponType::USSOCOM:
            name = "USSOCOM";
            ini_name = "USSOCOM";
            break;
        case WeaponType::Knife:
            name = "Combat Knife";
            ini_name = "Combat Knife";
            break;
        case WeaponType::Chainsaw:
            name = "Chainsaw";
            ini_name = "Chainsaw";
            break;
        case WeaponType::LAW:
            name = "LAW";
            ini_name = "M72 LAW";
            break;
        case WeaponType::FlameBow:
            name = "Flame Bow";
            ini_name = "Flamed Arrows";
            break;
        case WeaponType::Bow:
            name = "Bow";
            ini_name = "Rambo Bow";
            break;
        case WeaponType::Flamer:
            name = "Flamer";
            ini_name = "Flamer";
            break;
        case WeaponType::M2:
            name = "M2 MG";
            ini_name = "Stationary Gun";
            break;
        case WeaponType::NoWeapon:
            name = "Hands";
            ini_name = "Punch";
            break;
        case WeaponType::FragGrenade:
            name = "Frag Grenade";
            ini_name = "Grenade";
            break;
        case WeaponType::ClusterGrenade:
            name = "Frag Grenade";
            // TODO: it's not in ini
            ini_name = "";
            break;
        case WeaponType::Cluster:
            name = "Frag Grenade";
            // TODO: It's not in ini
            ini_name = "";
            break;
        case WeaponType::ThrownKnife:
            name = "Combat Knife";
            ini_name = "";
            break;
        default:
            std::unreachable();
    }

    hit_multiply = (float)ini_file.GetDoubleValue(ini_name.c_str(), "Damage");
    fire_interval = ini_file.GetLongValue(ini_name.c_str(), "FireInterval");
    ammo = ini_file.GetLongValue(ini_name.c_str(), "Ammo");
    reload_time = ini_file.GetLongValue(ini_name.c_str(), "ReloadTime");
    speed = (float)ini_file.GetDoubleValue(ini_name.c_str(), "Speed");
    // bullet_style = ConvertToBulletStyleEnum(ini_file.GetLongValue(ini_name, "BulletStyle"));
    start_up_time = ini_file.GetLongValue(ini_name.c_str(), "StartUpTime");
    bink = (int16_t)ini_file.GetLongValue(ini_name.c_str(), "Bink");
    movement_acc = (float)ini_file.GetDoubleValue(ini_name.c_str(), "MovementAcc");
    bullet_spread = (float)ini_file.GetDoubleValue(ini_name.c_str(), "BulletSpread");
    recoil = ini_file.GetLongValue(ini_name.c_str(), "Recoil");
    push = (float)ini_file.GetDoubleValue(ini_name.c_str(), "Push");
    inherited_velocity = (float)ini_file.GetDoubleValue(ini_name.c_str(), "InheritedVelocity");
    modifier_head = (float)ini_file.GetDoubleValue(ini_name.c_str(), "ModifierHead");
    modifier_chest = (float)ini_file.GetDoubleValue(ini_name.c_str(), "ModifierChest");
    modifier_legs = (float)ini_file.GetDoubleValue(ini_name.c_str(), "ModifierLegs");

    fire_interval_prev = fire_interval;
    fire_interval_count = fire_interval;
    fire_interval_real = (float)fire_interval;
    ammo_count = ammo;
    reload_time_prev = reload_time;
    reload_time_count = reload_time;
    reload_time_real = (float)reload_time;
    start_up_time_count = start_up_time;

    if (clip_reload) {
        clip_out_time = (std::uint16_t)((float)reload_time * 0.8);
        clip_in_time = (std::uint16_t)((float)reload_time * 0.3);
    } else {
        clip_out_time = 0;
        clip_in_time = 0;
    }

    // TODO: uncomment when bullet_style is implemented
    switch (bullet_style) {
        case BulletType::FragGrenade:
            timeout = GRENADE_TIMEOUT;
            break;
        case BulletType::ClusterGrenade:
            timeout = GRENADE_TIMEOUT;
            break;
        case BulletType::Flame:
            timeout = FLAMER_TIMEOUT;
            break;
        case BulletType::Fist:
            timeout = MELEE_TIMEOUT;
            break;
        case BulletType::Blade:
            timeout = MELEE_TIMEOUT;
            break;
        case BulletType::M2Bullet:
            timeout = M2BULLET_TIMEOUT;
            break;
        default:
            timeout = BULLET_TIMEOUT;
    }

    if (kind == WeaponType::M79) {
        ammo_count = 0;
    }

    std::cout << "Loaded weapon: " << name << std::endl;
    std::cout << hit_multiply << std::endl;
    std::cout << fire_interval << std::endl;
    // TODO: print more and check if loads correctly
}

bool Weapon::IsAny(const std::vector<WeaponType>& weapons) const
{
    return std::ranges::any_of(weapons,
                               [this](WeaponType weapon_type) { return weapon_type == kind; });
}
} // namespace Soldat
