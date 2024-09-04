#include "rendering/renderer/ItemRenderer.hpp"

#include "core/types/ItemType.hpp"
#include "rendering/data/sprites/SpriteTypes.hpp"
#include "rendering/shaders/ShaderSources.hpp"
#include "rendering/renderer/Renderer.hpp"

#include "core/math/Calc.hpp"

#include <glm/ext/quaternion_transform.hpp>
#include <numbers>
#include <cmath>

namespace Soldank
{
ItemRenderer::ItemRenderer(const Sprites::SpriteManager& sprite_manager)
    : shader_(ShaderSources::VERTEX_SHADER_SOURCE, ShaderSources::FRAGMENT_SHADER_SOURCE)
{
    // Flags
    LoadSpriteData(sprite_manager, ItemType::AlphaFlag);
    LoadSpriteData(sprite_manager, ItemType::BravoFlag);
    LoadSpriteData(sprite_manager, ItemType::PointmatchFlag);
    LoadObjectSpriteData(sprite_manager, Sprites::ObjectSpriteType::FlagHandle);

    // Kits
    LoadSpriteData(sprite_manager, ItemType::GrenadeKit);
    LoadSpriteData(sprite_manager, ItemType::MedicalKit);
    LoadSpriteData(sprite_manager, ItemType::VestKit);
    LoadSpriteData(sprite_manager, ItemType::ClusterKit);
    LoadSpriteData(sprite_manager, ItemType::PredatorKit);
    LoadSpriteData(sprite_manager, ItemType::BerserkKit);
    LoadSpriteData(sprite_manager, ItemType::FlamerKit);

    // Weapons
    LoadSpriteData(sprite_manager, ItemType::DesertEagles);
    LoadSpriteData(sprite_manager, ItemType::MP5);
    LoadSpriteData(sprite_manager, ItemType::Ak74);
    LoadSpriteData(sprite_manager, ItemType::SteyrAUG);
    LoadSpriteData(sprite_manager, ItemType::Spas12);
    LoadSpriteData(sprite_manager, ItemType::Ruger77);
    LoadSpriteData(sprite_manager, ItemType::M79);
    LoadSpriteData(sprite_manager, ItemType::Barrett);
    LoadSpriteData(sprite_manager, ItemType::Minimi);
    LoadSpriteData(sprite_manager, ItemType::Minigun);
    LoadSpriteData(sprite_manager, ItemType::USSOCOM);
    LoadSpriteData(sprite_manager, ItemType::Chainsaw);
    LoadSpriteData(sprite_manager, ItemType::Knife);
    LoadSpriteData(sprite_manager, ItemType::LAW);
    LoadSpriteData(sprite_manager, ItemType::Bow);
}

void ItemRenderer::LoadSpriteData(const Sprites::SpriteManager& sprite_manager, ItemType item_type)
{
    auto texture_data = sprite_manager.GetItemTexture(item_type);
    item_sprite_type_to_gl_data_[item_type] = texture_data;
}

void ItemRenderer::LoadObjectSpriteData(const Sprites::SpriteManager& sprite_manager,
                                        Sprites::ObjectSpriteType object_sprite_type)
{
    object_sprite_type_to_gl_data_[object_sprite_type] =
      sprite_manager.GetObjectSprite(object_sprite_type);
}

void ItemRenderer::Render(glm::mat4 transform, const Item& item, double frame_percent)
{
    if (!item.active) {
        return;
    }

    if (IsItemTypeFlag(item.style)) {
        // fade out (sort of)
        if (item.time_out < 300) {
            if ((item.time_out % 6) < 3) {
                return;
            }
        }
        RenderFlagSprites(transform, item, frame_percent);
        RenderQuad(transform, item, frame_percent);
    }

    if (IsItemTypeWeapon(item.style)) {
        RenderWeapon(transform, item, frame_percent);
    }

    if (IsItemTypeKit(item.style)) {
        RenderQuad(transform, item, frame_percent);
    }
}

void ItemRenderer::RenderQuad(glm::mat4 transform, const Item& item, double frame_percent)
{
    const auto& item_sprite_data = item_sprite_type_to_gl_data_.at(item.style);
    glm::vec2 pos =
      Calc::Lerp(item.skeleton->GetOldPos(1), item.skeleton->GetPos(1), (float)frame_percent);
    auto main_color = GetQuadMainColor(item.style);
    auto top_color = GetQuadTopColor(item.style);
    auto low_color = GetQuadLowColor(item.style);

    shader_.Use();

    // Set corners of the item on a (0,0) anchor from 1st corner
    glm::vec2 pos1 = item.skeleton->GetPos(1) - item.skeleton->GetPos(1);
    glm::vec2 pos2 = item.skeleton->GetPos(2) - item.skeleton->GetPos(1);
    glm::vec2 pos3 = item.skeleton->GetPos(3) - item.skeleton->GetPos(1);
    glm::vec2 pos4 = item.skeleton->GetPos(4) - item.skeleton->GetPos(1);

    if (IsItemTypeFlag(item.style)) {
        // Move up the position at the handle to halfway between it and the flag tip
        auto diff = pos2 - pos1;
        pos1 += Calc::Vec2Scale(diff, 0.5);
    }

    // OpenGL top screen is 1.0 but all the physics code has top screen at 0.0
    // So we need to flip the corners vertically (horizontal mirror image)
    pos1.y = -pos1.y;
    pos2.y = -pos2.y;
    pos3.y = -pos3.y;
    pos4.y = -pos4.y;

    // clang-format off
    std::vector<float> vertices{
      // position             // color                                          // texture
      pos1.x, pos1.y, 1.0F,   main_color.x, main_color.y, main_color.z, 1.0F,   0.0F, 0.0F,
      pos2.x, pos2.y, 1.0F,   low_color.x, low_color.y, low_color.z, 1.0F,   1.0F, 0.0F,
      pos3.x, pos3.y, 1.0F,   main_color.x, main_color.y, main_color.z, 1.0F,   1.0F, 1.0F,
      pos4.x, pos4.y, 1.0F,   top_color.x, top_color.y, top_color.z, 1.0F,   0.0F, 1.0F
    };
    // clang-format on

    glm::mat4 current_scenery_transform = transform;

    // We need to move the corners from (0,0) anchor to the position on the map
    current_scenery_transform =
      glm::translate(current_scenery_transform, glm::vec3(pos.x, -pos.y, 0.0));

    shader_.SetMatrix4("transform", current_scenery_transform);

    unsigned vbo = Renderer::CreateVBO(vertices, GL_STATIC_DRAW);
    std::vector<unsigned int> indices{ 0, 1, 3, 1, 2, 3 };
    unsigned int ebo = Renderer::CreateEBO(indices, GL_STATIC_DRAW);
    Renderer::SetupVertexArray(vbo, ebo, true, true);
    Renderer::BindTexture(item_sprite_data.opengl_id);
    Renderer::DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    Renderer::FreeVBO(vbo);
    Renderer::FreeEBO(ebo);
}

void ItemRenderer::RenderWeapon(glm::mat4 transform, const Item& item, double frame_percent)
{
    glm::vec2 position =
      Calc::Lerp(item.skeleton->GetOldPos(1), item.skeleton->GetPos(1), (float)frame_percent);
    float rotation = Calc::Vec2Angle(item.skeleton->GetPos(2) - position);
    glm::vec2 scale = { 1.0F, 1.0F };
    scale /= 4.5F;

    RenderSprite(transform, item_sprite_type_to_gl_data_.at(item.style), position, rotation, scale);
}

void ItemRenderer::RenderFlagSprites(glm::mat4 transform, const Item& item, double frame_percent)
{
    glm::vec2 position =
      Calc::Lerp(item.skeleton->GetOldPos(1), item.skeleton->GetPos(1), (float)frame_percent);
    float rotation = Calc::Vec2Angle(item.skeleton->GetPos(2) - position);
    glm::vec2 scale = { 1.0F, 1.0F };
    scale /= 4.5F;

    RenderSprite(transform,
                 object_sprite_type_to_gl_data_.at(Sprites::ObjectSpriteType::FlagHandle),
                 position,
                 rotation,
                 scale);
}

void ItemRenderer::RenderSprite(glm::mat4 transform,
                                const Texture::TextureData& item_sprite_data,
                                glm::vec2 position,
                                float rotation,
                                glm::vec2 scale)
{
    shader_.Use();
    float w0 = 0.0F;
    auto w1 = (float)item_sprite_data.width;
    float h0 = 0.0F;
    auto h1 = (float)item_sprite_data.height;
    glm::vec2 pos1 = { w0, h0 };
    glm::vec2 pos2 = { w1, h0 };
    glm::vec2 pos3 = { w1, h1 };
    glm::vec2 pos4 = { w0, h1 };

    // clang-format off
    std::vector<float> vertices{
      // position             // color                  // texture
      pos1.x, pos1.y, 1.0F,   1.0F, 1.0F, 1.0F, 1.0F,   0.0F, 0.0F,
      pos2.x, pos2.y, 1.0F,   1.0F, 1.0F, 1.0F, 1.0F,   1.0F, 0.0F,
      pos3.x, pos3.y, 1.0F,   1.0F, 1.0F, 1.0F, 1.0F,   1.0F, 1.0F,
      pos4.x, pos4.y, 1.0F,   1.0F, 1.0F, 1.0F, 1.0F,   0.0F, 1.0F
    };
    // clang-format on

    glm::mat4 current_scenery_transform = transform;

    // We need to move the corners from (0,0) anchor to the position on the map
    current_scenery_transform =
      glm::translate(current_scenery_transform, glm::vec3(position.x, -position.y, 0.0));
    current_scenery_transform =
      glm::rotate(current_scenery_transform, rotation, glm::vec3(0.0, 0.0, 1.0));
    current_scenery_transform =
      glm::scale(current_scenery_transform, glm::vec3(scale.x, scale.y, 0.0));

    shader_.SetMatrix4("transform", current_scenery_transform);

    unsigned vbo = Renderer::CreateVBO(vertices, GL_STATIC_DRAW);
    std::vector<unsigned int> indices{ 0, 1, 3, 1, 2, 3 };
    unsigned int ebo = Renderer::CreateEBO(indices, GL_STATIC_DRAW);
    Renderer::SetupVertexArray(vbo, ebo, true, true);
    Renderer::BindTexture(item_sprite_data.opengl_id);
    Renderer::DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    Renderer::FreeVBO(vbo);
    Renderer::FreeEBO(ebo);
}

glm::vec4 ItemRenderer::GetQuadMainColor(ItemType item_type)
{
    switch (item_type) {
        case ItemType::AlphaFlag:
            return { 173.0F / 255.0F, 21.0F / 255.0F, 21.0F / 255.0F, 1.0F };
        case ItemType::BravoFlag:
            return { 5.0F / 255.0F, 16.0F / 255.0F, 173.0F / 255.0F, 1.0F };
        case ItemType::PointmatchFlag:
            return { 173.0F / 255.0F, 173.0F / 255.0F, 21.0F / 255.0F, 1.0F };
        default:
            return { 1.0F, 1.0F, 1.0F, 1.0F };
    }
}

glm::vec4 ItemRenderer::GetQuadTopColor(ItemType item_type)
{
    switch (item_type) {
        case ItemType::AlphaFlag:
            return { 181.0F / 255.0F, 21.0F / 255.0F, 21.0F / 255.0F, 1.0F };
        case ItemType::BravoFlag:
            return { 5.0F / 255.0F, 16.0F / 255.0F, 181.0F / 255.0F, 1.0F };
        case ItemType::PointmatchFlag:
            return { 181.0F / 255.0F, 181.0F / 255.0F, 21.0F / 255.0F, 1.0F };
        default:
            return { 1.0F, 1.0F, 1.0F, 1.0F };
    }
}

glm::vec4 ItemRenderer::GetQuadLowColor(ItemType item_type)
{
    switch (item_type) {
        case ItemType::AlphaFlag:
            return { 149.0F / 255.0F, 21.0F / 255.0F, 21.0F / 255.0F, 1.0F };
        case ItemType::BravoFlag:
            return { 5.0F / 255.0F, 16.0F / 255.0F, 149.0F / 255.0F, 1.0F };
        case ItemType::PointmatchFlag:
            return { 149.0F / 255.0F, 149.0F / 255.0F, 21.0F / 255.0F, 1.0F };
        default:
            return { 1.0F, 1.0F, 1.0F, 1.0F };
    }
}
} // namespace Soldank
