#ifndef __SOLDIERRENDERER_HPP__
#define __SOLDIERRENDERER_HPP__

#include "rendering/shaders/Shader.hpp"
#include "rendering/data/sprites/SpritesManager.hpp"

#include "core/map/Map.hpp"
#include "core/entities/Soldier.hpp"

#include "core/math/Glm.hpp"

#include <vector>
#include <optional>

namespace Soldank
{
class SoldierRenderer
{
public:
    SoldierRenderer(const Sprites::SpriteManager& sprite_manager);
    ~SoldierRenderer();

    // it's not safe to be able to copy/move this because we would also need to take care of the
    // created OpenGL buffers and textures
    SoldierRenderer(const SoldierRenderer&) = delete;
    SoldierRenderer& operator=(SoldierRenderer other) = delete;
    SoldierRenderer(SoldierRenderer&&) = delete;
    SoldierRenderer& operator=(SoldierRenderer&& other) = delete;

    static void GenerateVertices(std::vector<float>& vertices,
                                 const Sprites::SoldierPartData& part_data,
                                 bool flipped);

    void Render(glm::mat4 transform,
                const Sprites::SpriteManager& sprite_manager,
                const Soldier& soldier,
                double frame_percent);

private:
    static bool IsSoldierPartTypeVisible(Sprites::SoldierPartSpriteType soldier_part_type,
                                         const Soldier& soldier,
                                         bool part_base_visibility);
    static bool IsPrimaryWeaponTypeVisible(
      Sprites::SoldierPartPrimaryWeaponSpriteType soldier_part_type,
      const Soldier& soldier,
      bool part_base_visibility);
    static bool IsSecondaryWeaponTypeVisible(
      Sprites::SoldierPartSecondaryWeaponSpriteType soldier_part_type,
      const Soldier& soldier);
    static bool IsTertiaryWeaponTypeVisible(
      Sprites::SoldierPartTertiaryWeaponSpriteType soldier_part_type,
      const Soldier& soldier);

    static glm::vec4 GetColorForSoldierPart(const Soldier& soldier,
                                            Sprites::SoldierSpriteColor soldier_color,
                                            Sprites::SoldierSpriteAlpha soldier_alpha);

    static const Weapon& GetPrimaryWeapon(const Soldier& soldier)
    {
        return soldier.weapons[soldier.active_weapon];
    }

    static const Weapon& GetSecondaryWeapon(const Soldier& soldier)
    {
        return soldier.weapons[(soldier.active_weapon + 1) % 2];
    }

    static const Weapon& GetTertiaryWeapon(const Soldier& soldier) { return soldier.weapons[2]; }

    Shader shader_;

    std::vector<std::pair<unsigned int, std::optional<unsigned int>>> vbos_;
    std::vector<unsigned int> ebos_;
    std::vector<Sprites::SoldierPartSpriteType> part_types_;
};
} // namespace Soldank

#endif
