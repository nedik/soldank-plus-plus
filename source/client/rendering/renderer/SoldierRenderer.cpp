#include "SoldierRenderer.hpp"

#include <math.h>

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

        GenerateVertices(vertices, part_data, false);
        unsigned int vbo_for_original = Renderer::CreateVBO(vertices, GL_STATIC_DRAW);
        std::optional<unsigned int> vbo_for_flipped = std::nullopt;
        if (part_data.IsFlippable()) {
            vertices.clear();
            GenerateVertices(vertices, part_data, true);
            vbo_for_flipped = Renderer::CreateVBO(vertices, GL_STATIC_DRAW);
        }

        vbos_.emplace_back(vbo_for_original, vbo_for_flipped);
        ebos_.push_back(Renderer::CreateEBO(indices, GL_STATIC_DRAW));
    }
}

SoldierRenderer::~SoldierRenderer()
{
    for (unsigned int ebo : ebos_) {
        Renderer::FreeEBO(ebo);
    }
    for (auto vbo : vbos_) {
        Renderer::FreeEBO(vbo.first);
        if (vbo.second.has_value()) {
            Renderer::FreeEBO(*vbo.second);
        }
    }
}

void SoldierRenderer::GenerateVertices(std::vector<float>& vertices,
                                       const Sprites::SoldierPartData& part_data,
                                       bool flipped)
{
    float texture_width = NAN;
    float texture_height = NAN;
    float pivot_x = part_data.GetCenter().x;
    float pivot_y = (1.0F - part_data.GetCenter().y);
    if (!flipped) {
        texture_width = (float)part_data.GetTextureWidth();
        texture_height = (float)part_data.GetTextureHeight();
    } else {
        texture_width = (float)part_data.GetTextureFlippedWidth();
        texture_height = (float)part_data.GetTextureFlippedHeight();
        pivot_y = 1.0F - pivot_y;
    }
    pivot_x *= texture_width;
    pivot_y *= texture_height;

    float w0 = 0.0F - pivot_x;
    float w1 = texture_width - pivot_x;
    float h0 = 0.0F - pivot_y;
    float h1 = texture_height - pivot_y;

    // clang-format off
        vertices = {
            // position         // color                    // texture
            w0, h0, 1.0,        1.0, 1.0, 1.0, 1.0,         0.0, 0.0,
            w1, h0, 1.0,        1.0, 1.0, 1.0, 1.0,         1.0, 0.0,
            w0, h1, 1.0,        1.0, 1.0, 1.0, 1.0,         0.0, 1.0,
            w1, h1, 1.0,        1.0, 1.0, 1.0, 1.0,         1.0, 1.0,
        };
    // clang-format on
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
        glm::vec2 scale = glm::vec2(1.0, 1.0);
        unsigned int px = part_data.GetPoint().x;
        unsigned int py = part_data.GetPoint().y;
        // TODO: frame_percent here causes soldier blurring when moving (sprite ghosting)
        glm::vec2 p0 = Calc::Lerp(
          soldier.skeleton->GetOldPos(px), soldier.skeleton->GetPos(px), (float)frame_percent);
        glm::vec2 p1 = Calc::Lerp(
          soldier.skeleton->GetOldPos(py), soldier.skeleton->GetPos(py), (float)frame_percent);
        float rot = Calc::Vec2Angle(p1 - p0);

        unsigned int vbo_to_use = vbos_[i].first;
        if (soldier.direction != 1) {
            if (part_data.IsFlippable()) {
                sprite_texture = part_data.GetTextureFlipped();
                vbo_to_use = *vbos_[i].second;
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

        Renderer::SetupVertexArray(vbo_to_use, ebos_[i], true);
        glm::mat4 current_part_transform = transform;

        current_part_transform =
          glm::translate(current_part_transform, glm::vec3(p0.x, -p0.y - 1.0, 0.0));
        current_part_transform = glm::rotate(current_part_transform, rot, glm::vec3(0.0, 0.0, 1.0));
        current_part_transform =
          glm::scale(current_part_transform, glm::vec3(scale.x, scale.y, 0.0));

        shader_.SetMatrix4("transform", current_part_transform);

        Renderer::BindTexture(sprite_texture);
        Renderer::DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, std::nullopt);
    }
}
} // namespace Soldat
