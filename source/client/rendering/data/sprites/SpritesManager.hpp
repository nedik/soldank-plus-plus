#ifndef __SPRITES_MANAGER_HPP__
#define __SPRITES_MANAGER_HPP__

#include "rendering/data/sprites/SpriteTypes.hpp"
#include "rendering/data/sprites/SoldierPartData.hpp"
#include "rendering/data/Texture.hpp"

#include "core/math/Glm.hpp"
#include "core/entities/Bullet.hpp"
#include "core/types/ItemType.hpp"

#include <string>
#include <vector>
#include <filesystem>
#include <optional>
#include <variant>
#include <unordered_map>

namespace Soldank::Sprites
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
    std::variant<SoldierPartSpriteType,
                 SoldierPartPrimaryWeaponSpriteType,
                 SoldierPartSecondaryWeaponSpriteType,
                 SoldierPartTertiaryWeaponSpriteType>
    GetSoldierPartDataType(unsigned int id) const;
    unsigned int GetSoldierPartCount() const;

    Texture::TextureData GetBulletTexture(WeaponSpriteType weapon_sprite_type) const;
    Texture::TextureData GetBulletTexture(const Bullet& bullet) const;
    Texture::TextureData GetItemTexture(ItemType item_type, bool flipped = false) const;
    Texture::TextureData GetObjectSprite(ObjectSpriteType object_sprite_type) const;

private:
    using TSoldierPartSpriteData = std::unique_ptr<SoldierPartData>;
    using TSoldierPartSpriteKey = std::variant<SoldierPartSpriteType,
                                               SoldierPartPrimaryWeaponSpriteType,
                                               SoldierPartSecondaryWeaponSpriteType,
                                               SoldierPartTertiaryWeaponSpriteType>;
    using TSoldierPartListItem = std::pair<TSoldierPartSpriteKey, TSoldierPartSpriteData>;
    using TSoldierPartList = std::vector<TSoldierPartListItem>;

    using TSpriteKey = std::variant<SoldierPartSpriteType, WeaponSpriteType, ObjectSpriteType>;

    void AddSprite(TSoldierPartSpriteKey soldier_part_sprite_key,
                   TSpriteKey sprite_key,
                   std::optional<TSpriteKey> flipped_sprite_key,
                   glm::uvec2 point,
                   glm::vec2 center,
                   bool visible,
                   bool team,
                   float flexibility,
                   SoldierSpriteColor color,
                   SoldierSpriteAlpha alpha);

    TSoldierPartList soldier_part_type_to_data_;

    std::unordered_map<TSpriteKey, Texture::TextureData> all_sprites_;
};
} // namespace Soldank::Sprites

#endif
