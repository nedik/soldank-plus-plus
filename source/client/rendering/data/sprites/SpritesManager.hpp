#ifndef __SPRITES_MANAGER_HPP__
#define __SPRITES_MANAGER_HPP__

#include "rendering/data/sprites/SpriteTypes.hpp"
#include "rendering/data/sprites/SoldierPartData.hpp"
#include "rendering/data/Texture.hpp"

#include "core/math/Glm.hpp"

#include <string>
#include <vector>
#include <filesystem>
#include <optional>
#include <variant>
#include <unordered_map>

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
    using TSoldierPartSpriteData = std::unique_ptr<SoldierPartData>;
    using TSoldierPartSpriteKey = std::variant<SoldierPartType,
                                               SoldierPartPrimaryWeaponType,
                                               SoldierPartSecondaryWeaponType,
                                               SoldierPartTertiaryWeaponType>;
    using TSoldierPartListItem = std::pair<TSoldierPartSpriteKey, TSoldierPartSpriteData>;
    using TSoldierPartList = std::vector<TSoldierPartListItem>;

    using TSpriteKey = std::variant<SoldierPartType, WeaponType>;

    void AddSprite(TSoldierPartSpriteKey soldier_part_sprite_key,
                   TSpriteKey sprite_key,
                   std::optional<TSpriteKey> flipped_sprite_key,
                   glm::uvec2 point,
                   glm::vec2 center,
                   bool visible,
                   bool team,
                   float flexibility,
                   SoldierColor color,
                   SoldierAlpha alpha);

    TSoldierPartList soldier_part_type_to_data_;

    std::unordered_map<TSpriteKey, Texture::TextureData> all_sprites_;
};
} // namespace Soldat::Sprites

#endif
