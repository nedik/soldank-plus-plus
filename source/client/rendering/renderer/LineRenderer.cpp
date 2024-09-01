#include "rendering/renderer/LineRenderer.hpp"

#include "rendering/renderer/Renderer.hpp"
#include "rendering/shaders/ShaderSources.hpp"

namespace Soldank
{
LineRenderer::LineRenderer()
    : shader_(ShaderSources::DYNAMIC_COLOR_NO_TEXTURE_VERTEX_SHADER_SOURCE,
              ShaderSources::DYNAMIC_COLOR_NO_TEXTURE_FRAGMENT_SHADER_SOURCE)
{
    std::vector<float> vertices;

    for (unsigned int i = 0; i < 4; i++) {
        vertices.push_back(0.0F);
        vertices.push_back(0.0F);
        vertices.push_back(0.0F);
    }

    vbo_ = Renderer::CreateVBO(vertices, GL_DYNAMIC_DRAW);
}

LineRenderer::~LineRenderer()
{
    Renderer::FreeVBO(vbo_);
}

void LineRenderer::Render(glm::mat4 transform,
                          float x1,
                          float y1,
                          float x2,
                          float y2,
                          float thickness)
{
    y1 = -y1;
    y2 = -y2;
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = sqrt(dx * dx + dy * dy);

    // Normalize direction vector
    dx /= length;
    dy /= length;

    // Perpendicular vector for thickness
    float px = -dy * thickness * 0.5F;
    float py = dx * thickness * 0.5F;

    // Vertex positions for a thick line (rectangle)
    // clang-format off
    std::vector<float> vertices{
        x1 + px, y1 + py, 1.0F,  // First vertex (offset from start point)
        x1 - px, y1 - py, 1.0F,  // Second vertex (opposite side of the start point)
        x2 + px, y2 + py, 1.0F,  // Third vertex (offset from end point)
        x2 - px, y2 - py, 1.0F   // Fourth vertex (opposite side of the end point)
    };
    // clang-format on

    shader_.Use();
    Renderer::SetupVertexArray(vbo_, std::nullopt, false, false);
    shader_.SetMatrix4("transform", transform);
    shader_.SetVec4("color", glm::vec4(1.0F, 1.0F, 1.0F, 1.0F));

    glBufferSubData(
      GL_ARRAY_BUFFER, 0, (long long)vertices.size() * (long long)sizeof(float), vertices.data());

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
} // namespace Soldank