#ifndef __RECTANGLE_RENDERER_HPP__
#define __RECTANGLE_RENDERER_HPP__

#include "rendering/shaders/Shader.hpp"

#include "core/math/Glm.hpp"

#include <vector>

namespace Soldank
{
class RectangleRenderer
{
public:
    RectangleRenderer();
    ~RectangleRenderer();

    // it's not safe to be able to copy/move this because we would also need to take care of the
    // created OpenGL buffers and textures
    RectangleRenderer(const RectangleRenderer&) = delete;
    RectangleRenderer& operator=(RectangleRenderer other) = delete;
    RectangleRenderer(RectangleRenderer&&) = delete;
    RectangleRenderer& operator=(RectangleRenderer&& other) = delete;

    void Render(glm::mat4 transform, glm::vec2 position);

private:
    Shader shader_;

    unsigned int vbo_;
};
} // namespace Soldank

#endif
