#ifndef __WEAPON_HPP__
#define __WEAPON_HPP__

#include "core/types/WeaponType.hpp"
#include "core/entities/WeaponParameters.hpp"

#include <vector>

namespace Soldat
{
class Weapon
{
public:
    Weapon(const WeaponParameters& weapon_parameters);

    bool IsAny(const std::vector<WeaponType>& weapons) const;
    const WeaponParameters& GetWeaponParameters() const;

    void ResetStartUpTimeCount() { start_up_time_count_ = weapon_parameters_.start_up_time; };
    void SetReloadTimePrev(std::uint16_t reload_time_prev)
    {
        reload_time_prev_ = reload_time_prev;
    };
    std::uint16_t GetReloadTimeCount() const { return reload_time_count_; };

    std::uint8_t GetAmmoCount() const { return ammo_count_; }

private:
    WeaponParameters weapon_parameters_;

    std::uint8_t ammo_count_;
    std::uint16_t fire_interval_prev_;
    std::uint16_t fire_interval_count_;
    float fire_interval_real_;
    std::uint16_t start_up_time_count_;
    std::uint16_t reload_time_prev_;
    std::uint16_t reload_time_count_;
    float reload_time_real_;
    std::uint16_t clip_in_time_;
    std::uint16_t clip_out_time_;
};
} // namespace Soldat

#endif
