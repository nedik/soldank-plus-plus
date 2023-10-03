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
class SpriteManager
{
public:
    SpriteManager();
    ~SpriteManager();

    // it's not safe to be able to copy/move this because we would also need to take care of the
    // created OpenGL textures
    SpriteManager(const SpriteManager&) = delete;
    SpriteManager& operator=(SpriteManager other) = delete;
    SpriteManager(SpriteManager&&) = delete;
    SpriteManager& operator=(SpriteManager&& other) = delete;

    const SoldierPartData* GetSoldierPartData(unsigned int id) const;
    std::variant<SoldierPartType,
                 SoldierPartPrimaryWeaponType,
                 SoldierPartSecondaryWeaponType,
                 SoldierPartTertiaryWeaponType>
    GetSoldierPartDataType(unsigned int id) const;
    unsigned int GetSoldierPartCount() const;

private:
    using TSpriteData = std::unique_ptr<SoldierPartData>;
    using TSpriteKey = std::variant<SoldierPartType,
                                    SoldierPartPrimaryWeaponType,
                                    SoldierPartSecondaryWeaponType,
                                    SoldierPartTertiaryWeaponType>;
    using TListItem = std::pair<TSpriteKey, TSpriteData>;
    using TList = std::vector<TListItem>;

    std::unique_ptr<TList> soldier_part_type_to_data_;
};
} // namespace Soldat::Sprites

#endif
