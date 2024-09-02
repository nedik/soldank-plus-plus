#ifndef __CIRCLE_RENDERER_HPP__
#define __CIRCLE_RENDERER_HPP__

#include "rendering/shaders/Shader.hpp"

#include "core/math/Glm.hpp"

namespace Soldank
{
class CircleRenderer
{
public:
    CircleRenderer();
    ~CircleRenderer();

    // it's not safe to be able to copy/move this because we would also need to take care of the
    // created OpenGL buffers and textures
    CircleRenderer(const CircleRenderer&) = delete;
    CircleRenderer& operator=(CircleRenderer other) = delete;
    CircleRenderer(CircleRenderer&&) = delete;
    CircleRenderer& operator=(CircleRenderer&& other) = delete;

    void Render(glm::mat4 transform,
                glm::vec2 position,
                glm::vec4 color,
                float outer_radius,
                float inner_radius = 0.0F);

private:
    Shader shader_;

    unsigned int vbo_;
};
} // namespace Soldank

#endif
