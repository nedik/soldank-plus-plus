#include "PolygonOutlinesRenderer.hpp"

#include "rendering/data/Texture.hpp"
#include "rendering/renderer/Renderer.hpp"
#include "rendering/shaders/ShaderSources.hpp"

#include <filesystem>
#include <iostream>

namespace Soldat
{
PolygonOutlinesRenderer::PolygonOutlinesRenderer(const std::vector<PMSPolygon>& polygons)
    : shader_(ShaderSources::NO_TEXTURE_VERTEX_SHADER_SOURCE,
              ShaderSources::NO_TEXTURE_FRAGMENT_SHADER_SOURCE)
    , polygons_count_(polygons.size())
{
    std::vector<float> vertices;

    for (const auto& polygon : polygons) {
        for (const auto& vertice : polygon.vertices) {
            vertices.push_back(vertice.x);
            vertices.push_back(-vertice.y);
            vertices.push_back(vertice.z);
            vertices.push_back(1.0); // red
            vertices.push_back(1.0); // green
            vertices.push_back(1.0); // blue
            vertices.push_back(1.0); // alpha
        }
    }

    vbo_ = Renderer::CreateVBO(vertices, GL_STATIC_DRAW);
}

PolygonOutlinesRenderer::~PolygonOutlinesRenderer()
{
    Renderer::FreeVBO(vbo_);
}

void PolygonOutlinesRenderer::Render(glm::mat4 transform, unsigned int polygon_id)
{
    shader_.Use();
    Renderer::SetupVertexArray(vbo_, std::nullopt, true, false);
    shader_.SetMatrix4("transform", transform);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // TODO: Move to Renderer if needed
    Renderer::DrawArrays(GL_TRIANGLES, (int)polygon_id * 3, 3);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
} // namespace Soldat