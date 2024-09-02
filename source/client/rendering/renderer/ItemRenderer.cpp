#include "rendering/renderer/ItemRenderer.hpp"

#include "rendering/data/sprites/SpriteTypes.hpp"
#include "rendering/shaders/ShaderSources.hpp"
#include "rendering/renderer/Renderer.hpp"

#include "core/math/Calc.hpp"

#include <numbers>
#include <cmath>

namespace Soldank
{
ItemRenderer::ItemRenderer(const Sprites::SpriteManager& sprite_manager)
    : shader_(ShaderSources::VERTEX_SHADER_SOURCE, ShaderSources::FRAGMENT_SHADER_SOURCE)
{
    LoadSpriteData(sprite_manager, ItemType::AlphaFlag, { 0.5, 0.5 });
    LoadSpriteData(sprite_manager, ItemType::GrenadeKit, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::MedicalKit, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::VestKit, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::ClusterKit, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::PredatorKit, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::BerserkKit, { 0.5F, 0.5F });
    LoadSpriteData(sprite_manager, ItemType::FlamerKit, { 0.5F, 0.5F });
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

    glm::vec2 scale = { 1.0F, 1.0F };
    ItemSpriteData item_sprite_data = item_sprite_type_to_gl_data_.at(item.style);
    glm::vec2 pos = item.skeleton->GetPos(1);
    auto main_color = GetMainColor(item.style);

    shader_.Use();

    // TODO: magic number, this is in mod.ini
    scale /= 4.5F;

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
      glm::translate(current_scenery_transform, glm::vec3(pos.x, -pos.y, 0.0));

    shader_.SetMatrix4("transform", current_scenery_transform);

    Renderer::DrawQuad(vertices, item_sprite_data.texture_data.opengl_id);
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
