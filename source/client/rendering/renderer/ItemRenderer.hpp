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
    ~ItemRenderer() = default;

    // it's not safe to be able to copy/move this because we would also need to take care of the
    // created OpenGL buffers and textures
    ItemRenderer(const ItemRenderer&) = delete;
    ItemRenderer& operator=(ItemRenderer other) = delete;
    ItemRenderer(ItemRenderer&&) = delete;
    ItemRenderer& operator=(ItemRenderer&& other) = delete;

    void Render(glm::mat4 transform, const Item& item, double frame_percent);

private:
    struct ItemSpriteData
    {
        unsigned int vbo;
        unsigned int ebo;
        Texture::TextureData texture_data;
    };

    void LoadSpriteData(const Sprites::SpriteManager& sprite_manager,
                        ItemType item_type,
                        glm::vec2 pivot);

    static glm::vec4 GetMainColor(ItemType item_type);

    Shader shader_;

    std::unordered_map<ItemType, ItemSpriteData> item_sprite_type_to_gl_data_;
};
} // namespace Soldank

#endif
