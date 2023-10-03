#ifndef __SPRITES_HPP__
#define __SPRITES_HPP__

#include "rendering/data/sprites/SpriteTypes.hpp"
#include "rendering/data/sprites/SoldierPartData.hpp"

#include "core/math/Glm.hpp"

#include <string>
#include <vector>
#include <filesystem>
#include <optional>
#include <variant>

namespace Soldat::Sprites
{
void Init();
const SoldierPartData* Get(unsigned int id);
std::variant<SoldierPartType,
             SoldierPartPrimaryWeaponType,
             SoldierPartSecondaryWeaponType,
             SoldierPartTertiaryWeaponType>
GetType(unsigned int id);
unsigned int GetSoldierPartCount();
void Free();
} // namespace Soldat::Sprites

#endif
