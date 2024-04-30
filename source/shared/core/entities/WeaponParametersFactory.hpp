#ifndef __WEAPON_FACTORY_HPP__
#define __WEAPON_FACTORY_HPP__

#include "core/types/WeaponType.hpp"
#include "core/entities/WeaponParameters.hpp"

#include "core/data/IFileReader.hpp"
#include "core/data/FileReader.hpp"

#include <SimpleIni.h>

#include <string>

namespace Soldank::WeaponParametersFactory
{
WeaponParameters LoadFromINI(const CSimpleIniA& ini_config, WeaponType weapon_type);
WeaponParameters LoadFromINIFile(const std::string& ini_file_path,
                                 WeaponType weapon_type,
                                 const IFileReader& file_reader = FileReader());
const WeaponParameters& GetParameters(WeaponType weapon_type,
                                      bool realistic,
                                      const IFileReader& file_reader = FileReader());
std::string GetNameForWeaponType(WeaponType weapon_type);
std::string GetININameForWeaponType(WeaponType weapon_type);
bool GetClipReload(WeaponType weapon_type);
} // namespace Soldank::WeaponParametersFactory

#endif
