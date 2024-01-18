#include "PolygonsRenderer.hpp"

#include "rendering/data/Texture.hpp"
#include "rendering/renderer/Renderer.hpp"
#include "rendering/shaders/ShaderSources.hpp"

#include <filesystem>

namespace Soldat
{
PolygonsRenderer::PolygonsRenderer(const std::vector<PMSPolygon>& polygons,
                                   const std::string& texture_name)
    : shader_(ShaderSources::VERTEX_SHADER_SOURCE, ShaderSources::FRAGMENT_SHADER_SOURCE)
    , polygons_count_(polygons.size())
{
    std::filesystem::path texture_path = "textures/" + texture_name;
    if (!std::filesystem::exists(texture_path)) {
        texture_path.replace_extension(".png");
    }

    texture_ = Texture::Load(texture_path.string().c_str()).opengl_id;

    std::vector<float> vertices;

    for (const auto& polygon : polygons) {
        for (const auto& vertice : polygon.vertices) {
            vertices.push_back(vertice.x);
            vertices.push_back(-vertice.y);
            vertices.push_back(vertice.z);
            vertices.push_back((float)vertice.color.red / 255.0F);
            vertices.push_back((float)vertice.color.green / 255.0F);
            vertices.push_back((float)vertice.color.blue / 255.0F);
            vertices.push_back((float)vertice.color.alpha / 255.0F);
            vertices.push_back(vertice.texture_s);
            vertices.push_back(vertice.texture_t);
        }
    }

    vbo_ = Renderer::CreateVBO(vertices, GL_STATIC_DRAW);
}

PolygonsRenderer::~PolygonsRenderer()
{
    Renderer::FreeVBO(vbo_);
}

void PolygonsRenderer::Render(glm::mat4 transform)
{
    Renderer::SetupVertexArray(vbo_, std::nullopt);
    shader_.Use();
    Renderer::BindTexture(texture_);
    shader_.SetMatrix4("transform", transform);
    Renderer::DrawArrays(GL_TRIANGLES, 0, (int)polygons_count_ * 3);
}
} // namespace Soldat