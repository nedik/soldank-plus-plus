#ifndef __BULLET_RENDERER_HPP__
#define __BULLET_RENDERER_HPP__

#include "rendering/data/Texture.hpp"
#include "rendering/data/sprites/SpriteTypes.hpp"
#include "rendering/shaders/Shader.hpp"
#include "rendering/data/sprites/SpritesManager.hpp"

#include "core/math/Glm.hpp"
#include "core/entities/Bullet.hpp"

#include <map>
#include <unordered_map>

namespace Soldat
{
class BulletRenderer
{
public:
    BulletRenderer(const Sprites::SpriteManager& sprite_manager);
    ~BulletRenderer() = default;

    // it's not safe to be able to copy/move this because we would also need to take care of the
    // created OpenGL buffers and textures
    BulletRenderer(const BulletRenderer&) = delete;
    BulletRenderer& operator=(BulletRenderer other) = delete;
    BulletRenderer(BulletRenderer&&) = delete;
    BulletRenderer& operator=(BulletRenderer&& other) = delete;

    void Render(glm::mat4 transform, const Bullet& bullet, double frame_percent);

private:
    struct BulletSpriteData
    {
        unsigned int vbo;
        unsigned int ebo;
        Texture::TextureData texture_data;
    };

    void LoadSpriteData(const Sprites::SpriteManager& sprite_manager,
                        Sprites::WeaponSpriteType weapon_sprite_type,
                        glm::vec2 pivot);

    Shader shader_;

    std::unordered_map<Sprites::WeaponSpriteType, BulletSpriteData> weapon_sprite_type_to_gl_data_;
};
} // namespace Soldat

#endif
