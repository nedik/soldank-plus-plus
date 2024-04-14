#include "CursorRenderer.hpp"

#include "rendering/data/Texture.hpp"
#include "rendering/renderer/Renderer.hpp"
#include "rendering/shaders/ShaderSources.hpp"

#include "core/math/Glm.hpp"

#include "spdlog/spdlog.h"

#include <filesystem>

namespace Soldank
{
CursorRenderer::CursorRenderer()
    : shader_(ShaderSources::VERTEX_SHADER_SOURCE, ShaderSources::FRAGMENT_SHADER_SOURCE)
{
    std::filesystem::path texture_path = "interface-gfx/cursor.png";
    auto texture_or_error = Texture::Load(texture_path.string().c_str());

    if (texture_or_error.has_value()) {
        texture_ = texture_or_error.value().opengl_id;
        texture_width_ = texture_or_error.value().width;
        texture_height_ = texture_or_error.value().height;
    } else {
        spdlog::critical("Texture file not found {}", texture_path.string());
        texture_ = 0;
        texture_width_ = 0;
        texture_height_ = 0;
    }
    float texture_width = (float)texture_width_ / 640.0F;
    float texture_height = (float)texture_height_ / 480.0F;

    // clang-format off
    std::vector<float> vertices{
        // position                                             // color               // texture
        -texture_width / 2.0F, -texture_height / 2.0F, 1.0,    1.0, 1.0, 1.0, 1.0,     0.0, 0.0,
        texture_width / 2.0F,  -texture_height / 2.0F, 1.0,    1.0, 1.0, 1.0, 1.0,     1.0, 0.0,
        -texture_width / 2.0F, texture_height / 2.0F,  1.0,    1.0, 1.0, 1.0, 1.0,     0.0, 1.0,
        texture_width / 2.0F,  texture_height / 2.0F,  1.0,    1.0, 1.0, 1.0, 1.0,     1.0, 1.0,
    };
    // clang-format on

    std::vector<unsigned int> indices{ 0, 1, 2, 1, 3, 2 };

    vbo_ = Renderer::CreateVBO(vertices, GL_STATIC_DRAW);
    ebo_ = Renderer::CreateEBO(indices, GL_STATIC_DRAW);
}

CursorRenderer::~CursorRenderer()
{
    Renderer::FreeVBO(vbo_);
    Renderer::FreeEBO(ebo_);
}

void CursorRenderer::Render(const glm::vec2& mouse_position)
{
    shader_.Use();
    ;
    Renderer::SetupVertexArray(vbo_, ebo_);

    glm::mat4 transform(1);
    transform = glm::translate(
      transform, glm::vec3(mouse_position.x / 320.0 - 1.0, mouse_position.y / 240.0 - 1.0, 0.0));
    transform = glm::scale(transform, glm::vec3(SIZE_SCALE, SIZE_SCALE, 0.0));

    shader_.SetMatrix4("transform", transform);
    Renderer::BindTexture(texture_);
    Renderer::DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
} // namespace Soldank
