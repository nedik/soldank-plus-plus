#include "Weapon.hpp"

#include <algorithm>
#include <utility>

namespace Soldat
{
Weapon::Weapon(const WeaponParameters& weapon_parameters)
    : weapon_parameters_(weapon_parameters)
    , ammo_count_(weapon_parameters.ammo)
    , fire_interval_prev_(weapon_parameters.fire_interval)
    , fire_interval_count_(weapon_parameters.fire_interval)
    , fire_interval_real_((float)weapon_parameters.fire_interval)
    , start_up_time_count_(weapon_parameters.start_up_time)
    , reload_time_prev_(weapon_parameters.reload_time)
    , reload_time_count_(weapon_parameters.reload_time)
    , reload_time_real_((float)weapon_parameters.reload_time)
{

    if (weapon_parameters.clip_reload) {
        clip_out_time_ = (std::uint16_t)((float)weapon_parameters.reload_time * 0.8);
        clip_in_time_ = (std::uint16_t)((float)weapon_parameters.reload_time * 0.3);
    } else {
        clip_out_time_ = 0;
        clip_in_time_ = 0;
    }

    if (weapon_parameters.kind == WeaponType::M79) {
        ammo_count_ = 0;
    }
}

bool Weapon::IsAny(const std::vector<WeaponType>& weapons) const
{
    return std::ranges::any_of(weapons, [this](WeaponType weapon_type) {
        return weapon_type == GetWeaponParameters().kind;
    });
}

const WeaponParameters& Weapon::GetWeaponParameters() const
{
    return weapon_parameters_;
}

} // namespace Soldat
