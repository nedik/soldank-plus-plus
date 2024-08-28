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
    float rot = 1.5708F; // 90 degrees
    auto main_color = GetMainColor(item.style);

    shader_.Use();
    // Renderer::SetupVertexArray(item_sprite_data.vbo, item_sprite_data.ebo, false, true);

    // TODO: magic number, this is in mod.ini
    scale /= 4.5F;

    glm::mat4 current_scenery_transform = transform;

    current_scenery_transform =
      glm::translate(current_scenery_transform, glm::vec3(pos.x, -pos.y, 0.0));
    current_scenery_transform =
      glm::rotate(current_scenery_transform, rot, glm::vec3(0.0, 0.0, 1.0));
    current_scenery_transform =
      glm::scale(current_scenery_transform, glm::vec3(scale.x, scale.y, 0.0));

    shader_.SetMatrix4("transform", current_scenery_transform);
    // shader_.SetVec4("color", main_color);

    // Renderer::BindTexture(item_sprite_data.texture_data.opengl_id);
    // Renderer::DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glm::vec2 pivot;
    pivot.x = (float)item_sprite_data.texture_data.width / 2.0F;
    pivot.y = (float)item_sprite_data.texture_data.height / 2.0F;
    float w0 = 0.0F - pivot.x;
    float w1 = (float)item_sprite_data.texture_data.width - pivot.x;
    float h0 = 0.0F - pivot.y;
    float h1 = (float)item_sprite_data.texture_data.height - pivot.y;
    // float w0 = 0.0F;
    // float w1 = (float)item_sprite_data.texture_data.width;
    // float h0 = 0.0F;
    // float h1 = (float)item_sprite_data.texture_data.height;
    glm::vec2 pos1 = { w0, h0 };
    glm::vec2 pos2 = { w1, h0 };
    glm::vec2 pos3 = { w0, h1 };
    glm::vec2 pos4 = { w1, h1 };
    // pos1 = item.skeleton->GetPos(1);
    // pos2 = item.skeleton->GetPos(2);
    // pos3 = item.skeleton->GetPos(3);
    // pos4 = item.skeleton->GetPos(4);

    // clang-format off
    std::vector<float> vertices{
      // position             // color                  // texture
      pos1.x, pos1.y, 1.0F,   1.0F, 1.0F, 1.0F, 1.0F,   0.0F, 0.0F,
      pos2.x, pos2.y, 1.0F,   1.0F, 1.0F, 1.0F, 1.0F,   1.0F, 0.0F,
      pos3.x, pos3.y, 1.0F,   1.0F, 1.0F, 1.0F, 1.0F,   0.0F, 1.0F,
      pos4.x, pos4.y, 1.0F,   1.0F, 1.0F, 1.0F, 1.0F,   1.0F, 1.0F
    };
    // std::vector<float> vertices{
    //   // position             // color                  // texture
    //   pos2.x, pos2.y, 1.0F,   1.0F, 1.0F, 1.0F, 1.0F,   0.0F, 1.0F,
    //   pos1.x, pos1.y, 1.0F,   1.0F, 1.0F, 1.0F, 1.0F,   0.0F, 0.0F,
    //   pos4.x, pos4.y, 1.0F,   1.0F, 1.0F, 1.0F, 1.0F,   1.0F, 0.0F,
    //   pos3.x, pos3.y, 1.0F,   1.0F, 1.0F, 1.0F, 1.0F,   1.0F, 1.0F
    // };
    // clang-format on
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
