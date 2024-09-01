#ifndef __LINE_RENDERER_HPP__
#define __LINE_RENDERER_HPP__

#include "rendering/shaders/Shader.hpp"

#include "core/math/Glm.hpp"

namespace Soldank
{
class LineRenderer
{
public:
    LineRenderer();
    ~LineRenderer();

    // it's not safe to be able to copy/move this because we would also need to take care of the
    // created OpenGL buffers and textures
    LineRenderer(const LineRenderer&) = delete;
    LineRenderer& operator=(LineRenderer other) = delete;
    LineRenderer(LineRenderer&&) = delete;
    LineRenderer& operator=(LineRenderer&& other) = delete;

    void Render(glm::mat4 transform, float x1, float y1, float x2, float y2, float thickness);

private:
    Shader shader_;

    unsigned int vbo_;
};
} // namespace Soldank

#endif
