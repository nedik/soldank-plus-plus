#ifndef __CURSORRENDERER_HPP__
#define __CURSORRENDERER_HPP__

#include "rendering/shaders/Shader.hpp"

#include "core/math/Glm.hpp"

namespace Soldank
{
class CursorRenderer
{
public:
    CursorRenderer();
    ~CursorRenderer();

    // it's not safe to be able to copy/move this because we would also need to take care of the
    // created OpenGL buffers and textures
    CursorRenderer(const CursorRenderer&) = delete;
    CursorRenderer& operator=(CursorRenderer other) = delete;
    CursorRenderer(CursorRenderer&&) = delete;
    CursorRenderer& operator=(CursorRenderer&& other) = delete;

    void Render(const glm::vec2& mouse_position);

private:
    // TODO: this should be taken from mod.ini
    constexpr static const float SIZE_SCALE = 0.1;

    Shader shader_;

    unsigned int texture_;
    int texture_width_;
    int texture_height_;
    unsigned int vbo_;
    unsigned int ebo_;
};
} // namespace Soldank

#endif
