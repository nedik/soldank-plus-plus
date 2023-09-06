#ifndef __WEAPON_FACTORY_HPP__
#define __WEAPON_FACTORY_HPP__

#include "core/types/WeaponType.hpp"
#include "core/entities/WeaponParameters.hpp"

#include <SimpleIni.h>

#include <string>

namespace Soldat::WeaponParametersFactory
{
WeaponParameters LoadFromINI(const CSimpleIniA& ini_config, WeaponType weapon_type);
WeaponParameters LoadFromINIFile(const std::string& ini_file_path, WeaponType weapon_type);
const WeaponParameters& GetParameters(WeaponType weapon_type, bool realistic);
std::string GetNameForWeaponType(WeaponType weapon_type);
std::string GetININameForWeaponType(WeaponType weapon_type);
bool GetClipReload(WeaponType weapon_type);
} // namespace Soldat::WeaponParametersFactory

#endif
