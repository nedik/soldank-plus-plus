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
    LoadSpriteData(sprite_manager, ItemType::AlphaFlag, { 0.5, 0.5 });
    LoadSpriteData(sprite_manager, ItemType::BravoFlag, { 0.5, 0.5 });
    LoadSpriteData(sprite_manager, ItemType::PointmatchFlag, { 0.5, 0.5 });

    // Kits
    LoadSpriteData(sprite_manager, ItemType::GrenadeKit, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::MedicalKit, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::VestKit, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::ClusterKit, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::PredatorKit, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::BerserkKit, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::FlamerKit, { 0.5F, 0.5F });

    // Weapons
    LoadSpriteData(sprite_manager, ItemType::DesertEagles, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::MP5, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::Ak74, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::SteyrAUG, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::Spas12, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::Ruger77, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::M79, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::Barrett, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::Minimi, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::Minigun, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::USSOCOM, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::Chainsaw, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::Knife, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::LAW, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::Bow, { 0.5F, 0.5F });
}

void ItemRenderer::LoadSpriteData(const Sprites::SpriteManager& sprite_manager,
                                  ItemType item_type,
                                  glm::vec2 pivot)
{
    auto texture_data = sprite_manager.GetItemTexture(item_type);

    pivot.x *= (float)texture_data.width;
    pivot.y *= (float)texture_data.height;
    float w0 = 0.0F - pivot.x;
    float w1 = (float)texture_data.width - pivot.x;
    float h0 = 0.0F - pivot.y;
    float h1 = (float)texture_data.height - pivot.y;

    // clang-format off
    std::vector<float> vertices{
      // position     // texture
      w0, h0, 1.0F,   0.0F, 0.0F,
      w1, h0, 1.0F,   1.0F, 0.0F,
      w0, h1, 1.0F,   0.0F, 1.0F,
      w1, h1, 1.0F,   1.0F, 1.0F
    };
    // clang-format on

    std::vector<unsigned int> indices{ 0, 1, 2, 1, 3, 2 };

    unsigned int vbo = Renderer::CreateVBO(vertices, GL_STATIC_DRAW);
    unsigned int ebo = Renderer::CreateEBO(indices, GL_STATIC_DRAW);
    item_sprite_type_to_gl_data_[item_type] = { vbo, ebo, texture_data };
}

void ItemRenderer::Render(glm::mat4 transform, const Item& item, double frame_percent)
{
    if (!item.active) {
        return;
    }

    if (IsItemTypeFlag(item.style)) {
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
    ItemSpriteData item_sprite_data = item_sprite_type_to_gl_data_.at(item.style);
    glm::vec2 pos = item.skeleton->GetPos(1);
    auto main_color = GetMainColor(item.style);

    shader_.Use();

    // Set corners of the item on a (0,0) anchor from 1st corner
    glm::vec2 pos1 = item.skeleton->GetPos(1) - item.skeleton->GetPos(1);
    glm::vec2 pos2 = item.skeleton->GetPos(2) - item.skeleton->GetPos(1);
    glm::vec2 pos3 = item.skeleton->GetPos(3) - item.skeleton->GetPos(1);
    glm::vec2 pos4 = item.skeleton->GetPos(4) - item.skeleton->GetPos(1);
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
      pos2.x, pos2.y, 1.0F,   main_color.x, main_color.y, main_color.z, 1.0F,   1.0F, 0.0F,
      pos3.x, pos3.y, 1.0F,   main_color.x, main_color.y, main_color.z, 1.0F,   1.0F, 1.0F,
      pos4.x, pos4.y, 1.0F,   main_color.x, main_color.y, main_color.z, 1.0F,   0.0F, 1.0F
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
    Renderer::BindTexture(item_sprite_data.texture_data.opengl_id);
    Renderer::DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    Renderer::FreeVBO(vbo);
    Renderer::FreeEBO(ebo);
}

void ItemRenderer::RenderWeapon(glm::mat4 transform, const Item& item, double frame_percent)
{
    glm::vec2 pos = item.skeleton->GetPos(1);
    float rotation = Calc::Vec2Angle(item.skeleton->GetPos(2) - pos);

    ItemSpriteData item_sprite_data = item_sprite_type_to_gl_data_.at(item.style);
    float w0 = 0.0F;
    auto w1 = (float)item_sprite_data.texture_data.width;
    float h0 = 0.0F;
    auto h1 = (float)item_sprite_data.texture_data.height;
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

    glm::vec2 scale = { 1.0F, 1.0F };
    scale /= 4.5F;

    // We need to move the corners from (0,0) anchor to the position on the map
    current_scenery_transform =
      glm::translate(current_scenery_transform, glm::vec3(pos.x, -pos.y, 0.0));
    current_scenery_transform =
      glm::rotate(current_scenery_transform, rotation, glm::vec3(0.0, 0.0, 1.0));
    current_scenery_transform =
      glm::scale(current_scenery_transform, glm::vec3(scale.x, scale.y, 0.0));

    shader_.SetMatrix4("transform", current_scenery_transform);

    unsigned vbo = Renderer::CreateVBO(vertices, GL_STATIC_DRAW);
    std::vector<unsigned int> indices{ 0, 1, 3, 1, 2, 3 };
    unsigned int ebo = Renderer::CreateEBO(indices, GL_STATIC_DRAW);
    Renderer::SetupVertexArray(vbo, ebo, true, true);
    Renderer::BindTexture(item_sprite_data.texture_data.opengl_id);
    Renderer::DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    Renderer::FreeVBO(vbo);
    Renderer::FreeEBO(ebo);
}

glm::vec4 ItemRenderer::GetMainColor(ItemType item_type)
{
    switch (item_type) {
        case ItemType::AlphaFlag:
            return { 173.0F / 255.0F, 21.0F / 255.0F, 21.0F / 255.0F, 1.0F };
        case ItemType::BravoFlag:
        case ItemType::PointmatchFlag:
        case ItemType::USSOCOM:
        case ItemType::DesertEagles:
        case ItemType::MP5:
        case ItemType::Ak74:
        case ItemType::SteyrAUG:
        case ItemType::Spas12:
        case ItemType::Ruger77:
        case ItemType::M79:
        case ItemType::Barrett:
        case ItemType::Minimi:
        case ItemType::Minigun:
        case ItemType::Bow:
        case ItemType::MedicalKit:
        case ItemType::GrenadeKit:
        case ItemType::FlamerKit:
        case ItemType::PredatorKit:
        case ItemType::VestKit:
        case ItemType::BerserkKit:
        case ItemType::ClusterKit:
        case ItemType::Parachute:
        case ItemType::Knife:
        case ItemType::Chainsaw:
        case ItemType::LAW:
        case ItemType::M2:
            return { 1.0F, 1.0F, 1.0F, 1.0F };
    }
}
} // namespace Soldank
