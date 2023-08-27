#ifndef __WEAPONS_HPP__
#define __WEAPONS_HPP__

#include "core/types/BulletType.hpp"
#include "core/types/WeaponType.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace Soldat
{
class Weapon
{
public:
    Weapon(WeaponType kind, bool realistic);

    bool IsAny(const std::vector<WeaponType>& weapons) const;

    WeaponType kind;
    std::uint8_t ammo;
    std::uint8_t ammo_count;
    float movement_acc;
    std::int16_t bink;
    std::uint16_t recoil;
    std::uint16_t fire_interval;
    std::uint16_t fire_interval_prev;
    std::uint16_t fire_interval_count;
    float fire_interval_real;
    std::uint16_t start_up_time;
    std::uint16_t start_up_time_count;
    std::uint16_t reload_time;
    std::uint16_t reload_time_prev;
    std::uint16_t reload_time_count;
    float reload_time_real;
    bool clip_reload;
    std::uint16_t clip_in_time;
    std::uint16_t clip_out_time;
    std::string name;
    std::string ini_name;
    float speed;
    float hit_multiply;
    float bullet_spread;
    float push;
    float inherited_velocity;
    float modifier_legs;
    float modifier_chest;
    float modifier_head;
    std::uint8_t fire_mode;
    std::uint16_t timeout;
    // TODO
    BulletType bullet_style;
    // Option<gfx::Weapon> sprite;
    // Option<gfx::Weapon> clip_sprite;
    // Option<gfx::Weapon> fire_sprite;
    // Option<gfx::Weapon> bullet_sprite;
};
} // namespace Soldat

#endif
