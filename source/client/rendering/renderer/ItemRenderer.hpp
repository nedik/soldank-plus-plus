#ifndef __ITEM_RENDERER_HPP__
#define __ITEM_RENDERER_HPP__

#include "rendering/data/Texture.hpp"
#include "rendering/data/sprites/SpriteTypes.hpp"
#include "rendering/shaders/Shader.hpp"
#include "rendering/data/sprites/SpritesManager.hpp"

#include "core/math/Glm.hpp"
#include "core/entities/Item.hpp"

#include <map>
#include <unordered_map>

namespace Soldank
{
class ItemRenderer
{
public:
    ItemRenderer(const Sprites::SpriteManager& sprite_manager);
    ~ItemRenderer();

    // it's not safe to be able to copy/move this because we would also need to take care of the
    // created OpenGL buffers and textures
    ItemRenderer(const ItemRenderer&) = delete;
    ItemRenderer& operator=(ItemRenderer other) = delete;
    ItemRenderer(ItemRenderer&&) = delete;
    ItemRenderer& operator=(ItemRenderer&& other) = delete;

    void Render(glm::mat4 transform,
                const Item& item,
                double frame_percent,
                unsigned int game_tick);

private:
    struct ItemSpriteData
    {
        unsigned int vbo;
        unsigned int ebo;
        Texture::TextureData texture_data;
    };

    void LoadSpriteData(const Sprites::SpriteManager& sprite_manager, ItemType item_type);
    void LoadObjectSpriteData(const Sprites::SpriteManager& sprite_manager,
                              Sprites::ObjectSpriteType object_sprite_type);

    void RenderQuad(glm::mat4 transform, const Item& item, double frame_percent);
    void RenderWeapon(glm::mat4 transform, const Item& item, double frame_percent);
    void RenderFlagSprites(glm::mat4 transform,
                           const Item& item,
                           double frame_percent,
                           unsigned int game_tick);
    void RenderParachute(glm::mat4 transform, const Item& item, double frame_percent);
    void RenderSprite(glm::mat4 transform,
                      const Texture::TextureData& item_sprite_data,
                      glm::vec2 position,
                      float rotation,
                      glm::vec2 scale,
                      glm::vec4 color = { 1.0F, 1.0F, 1.0F, 1.0F },
                      glm::vec2 pivot = { 0.0F, 0.0F });

    static glm::vec4 GetQuadMainColor(ItemType item_type);
    static glm::vec4 GetQuadTopColor(ItemType item_type);
    static glm::vec4 GetQuadLowColor(ItemType item_type);

    Shader shader_;

    std::unordered_map<ItemType, Texture::TextureData> item_sprite_type_to_gl_data_;
    std::unordered_map<ItemType, Texture::TextureData> item_sprite_type_to_flipped_gl_data_;
    std::unordered_map<Sprites::ObjectSpriteType, Texture::TextureData>
      object_sprite_type_to_gl_data_;

    unsigned vbo_;
    unsigned ebo_;
};
} // namespace Soldank

#endif
