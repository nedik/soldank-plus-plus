#include "SoldierRenderer.hpp"

#include "rendering/data/Texture.hpp"
#include "rendering/renderer/Renderer.hpp"
#include "rendering/shaders/ShaderSources.hpp"

#include "core/math/Calc.hpp"

#include <filesystem>
#include <iostream>
#include <cmath>

namespace Soldat
{
SoldierRenderer::SoldierRenderer()
    : shader_(ShaderSources::VERTEX_SHADER_SOURCE, ShaderSources::FRAGMENT_SHADER_SOURCE)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices{ 0, 1, 2, 1, 3, 2 };

    for (unsigned int i = 0; i < Sprites::GetSoldierPartCount(); i++) {
        vertices.clear();

        const Sprites::SoldierPartData& part_data = Sprites::Get(i);

        // TODO: flipped textures should be handled differently
        auto w = (float)part_data.GetTextureWidth();  // / 640.0f;
        auto h = (float)part_data.GetTextureHeight(); // / 480.0f;
        float dw = part_data.GetCenter().x * w;
        float dh = part_data.GetCenter().y * h;

        float w0 = 0.0; // - dw;
        float w1 = w;   // - dw;
        float h0 = 0.0; // - dh;
        float h1 = h;   // - dh;

        // clang-format off
        vertices = {
            // position         // color                    // texture
            w0, h0, 1.0,        1.0, 1.0, 1.0, 1.0,         0.0, 0.0,
            w1, h0, 1.0,        1.0, 1.0, 1.0, 1.0,         1.0, 0.0,
            w0, h1, 1.0,        1.0, 1.0, 1.0, 1.0,         0.0, 1.0,
            w1, h1, 1.0,        1.0, 1.0, 1.0, 1.0,         1.0, 1.0,
        };
        // clang-format on

        vbos_.push_back(Renderer::CreateVBO(vertices, GL_STATIC_DRAW));
        ebos_.push_back(Renderer::CreateEBO(indices, GL_STATIC_DRAW));
    }
}

SoldierRenderer::~SoldierRenderer()
{
    for (unsigned int ebo : ebos_) {
        Renderer::FreeEBO(ebo);
    }
    for (unsigned int vbo : vbos_) {
        Renderer::FreeEBO(vbo);
    }
}

void SoldierRenderer::Render(glm::mat4 transform, const Soldier& soldier, double frame_percent)
{
    shader_.Use();

    for (unsigned int i = 0; i < vbos_.size(); i++) {
        const Sprites::SoldierPartData& part_data = Sprites::Get(i);

        if (!part_data.IsVisible()) {
            // TODO: zaimplementować zmienianie widzialności części
            continue;
        }

        unsigned int sprite_texture = part_data.GetTexture();
        float cx = part_data.GetCenter().x;
        float cy = 1.0F - part_data.GetCenter().y;
        glm::vec2 scale = glm::vec2(1.0, 1.0);
        unsigned int px = part_data.GetPoint().x;
        unsigned int py = part_data.GetPoint().y;
        // TODO: frame_percent here causes soldier blurring when moving (sprite ghosting)
        glm::vec2 p0 = Calc::Lerp(
          soldier.skeleton->GetOldPos(px), soldier.skeleton->GetPos(px), (float)frame_percent);
        glm::vec2 p1 = Calc::Lerp(
          soldier.skeleton->GetOldPos(py), soldier.skeleton->GetPos(py), (float)frame_percent);
        float rot = Calc::Vec2Angle(p1 - p0);

        if (soldier.direction != 1) {
            if (part_data.IsFlippable()) {
                cy = 1.0F - cy;
                sprite_texture = part_data.GetTextureFlipped();
            } else {
                scale.y = -1.0;
            }
        }

        if (part_data.GetFlexibility() > 0.0) {
            scale.x = std::min(1.5F, glm::length(p1 - p0) / part_data.GetFlexibility());
        }

        // TODO: magic numbers, this is in mod.ini
        scale.x /= 4.5;
        scale.y /= 4.5;

        Renderer::SetupVertexArray(vbos_[i], ebos_[i], true);
        glm::mat4 current_part_transform = transform;
        glm::vec3 pivot = glm::vec3(
          cx * (float)part_data.GetTextureWidth(), cy * (float)part_data.GetTextureHeight(), 0.0);

        current_part_transform =
          glm::translate(current_part_transform, glm::vec3(p0.x, -p0.y - 1.0, 0.0));
        current_part_transform = glm::rotate(current_part_transform, rot, glm::vec3(0.0, 0.0, 1.0));
        current_part_transform =
          glm::scale(current_part_transform, glm::vec3(scale.x, scale.y, 0.0));
        current_part_transform = glm::translate(current_part_transform, -pivot);

        shader_.SetMatrix4("transform", current_part_transform);

        Renderer::BindTexture(sprite_texture);
        Renderer::DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, std::nullopt);
    }
}
} // namespace Soldat
