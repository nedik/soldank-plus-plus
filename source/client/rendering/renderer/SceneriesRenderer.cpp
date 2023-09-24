#include "SceneriesRenderer.hpp"

#include "rendering/data/Texture.hpp"
#include "rendering/renderer/Renderer.hpp"
#include "rendering/shaders/ShaderSources.hpp"

#include <filesystem>
#include <iostream>

namespace Soldat
{
SceneriesRenderer::SceneriesRenderer(const std::vector<PMSSceneryType>& scenery_types,
                                     const std::vector<PMSScenery>& scenery_instances)
    : shader_(ShaderSources::VERTEX_SHADER_SOURCE, ShaderSources::FRAGMENT_SHADER_SOURCE)
{
    for (auto scnery_type : scenery_types) {
        std::filesystem::path texture_path = "scenery-gfx/";
        texture_path += scnery_type.name;
        // if (!std::filesystem::exists(texture_path)) {
        //     texture_path.replace_extension(".png");
        // }

        textures_.push_back(Texture::Load(texture_path.string().c_str()));
    }

    std::vector<float> vertices;
    for (int i = 0; i < scenery_instances.size(); i++) {
        for (int j = 0; j < 4; j++) {
            vertices.push_back(0.0);
            vertices.push_back(0.0);
            vertices.push_back(1.0);
            vertices.push_back((float)scenery_instances[i].color.red / 255.0F);
            vertices.push_back((float)scenery_instances[i].color.green / 255.0F);
            vertices.push_back((float)scenery_instances[i].color.blue / 255.0F);
            vertices.push_back((float)scenery_instances[i].alpha / 255.0F);
            vertices.push_back(0.0);
            vertices.push_back(0.0);
        }

        vertices[i * 9 * 4 + 0] = 0.0;
        vertices[i * 9 * 4 + 1] = (float)-scenery_instances[i].height;
        vertices[i * 9 * 4 + 7] = 0.0;
        vertices[i * 9 * 4 + 8] = 0.0;

        vertices[i * 9 * 4 + 9] = (float)scenery_instances[i].width;
        vertices[i * 9 * 4 + 10] = (float)-scenery_instances[i].height;
        vertices[i * 9 * 4 + 16] = 1.0;
        vertices[i * 9 * 4 + 17] = 0.0;

        vertices[i * 9 * 4 + 18] = 0.0;
        vertices[i * 9 * 4 + 19] = 0.0;
        vertices[i * 9 * 4 + 25] = 0.0;
        vertices[i * 9 * 4 + 26] = 1.0;

        vertices[i * 9 * 4 + 27] = (float)scenery_instances[i].width;
        vertices[i * 9 * 4 + 28] = 0.0;
        vertices[i * 9 * 4 + 34] = 1.0;
        vertices[i * 9 * 4 + 35] = 1.0;
    }

    std::vector<unsigned int> indices;

    for (int i = 0; i < scenery_instances.size(); i++) {
        indices.push_back(i * 4 + 0);
        indices.push_back(i * 4 + 1);
        indices.push_back(i * 4 + 2);
        indices.push_back(i * 4 + 1);
        indices.push_back(i * 4 + 3);
        indices.push_back(i * 4 + 2);
    }

    vbo_ = Renderer::CreateVBO(vertices, GL_STATIC_DRAW);
    ebo_ = Renderer::CreateEBO(indices, GL_STATIC_DRAW);
}

SceneriesRenderer::~SceneriesRenderer()
{
    Renderer::FreeVBO(vbo_);
    Renderer::FreeVBO(ebo_);
}

void SceneriesRenderer::Render(glm::mat4 transform,
                               int target_level,
                               const std::vector<PMSScenery>& scenery_instances)
{
    shader_.Use();
    Renderer::SetupVertexArray(vbo_, ebo_);

    for (unsigned int i = 0; i < scenery_instances.size(); i++) {
        if (scenery_instances[i].level != target_level) {
            continue;
        }

        glm::mat4 current_scenery_transform = transform;

        current_scenery_transform =
          glm::translate(current_scenery_transform,
                         glm::vec3(scenery_instances[i].x, -scenery_instances[i].y, 0.0));
        current_scenery_transform = glm::rotate(
          current_scenery_transform, scenery_instances[i].rotation, glm::vec3(0.0, 0.0, 1.0));
        current_scenery_transform =
          glm::scale(current_scenery_transform,
                     glm::vec3(scenery_instances[i].scale_x, scenery_instances[i].scale_y, 0.0));

        shader_.SetMatrix4("transform", current_scenery_transform);

        Renderer::BindTexture(textures_[scenery_instances[i].style - 1]);
        Renderer::DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (i * 6ULL * sizeof(GLuint)));
    }
}
} // namespace Soldat
