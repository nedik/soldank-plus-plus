#ifndef __BACKGROUNDRENDERER_HPP__
#define __BACKGROUNDRENDERER_HPP__

#include "rendering/shaders/Shader.hpp"

#include "core/map/Map.hpp"
#include "core/math/Glm.hpp"

#include <vector>
#include <span>

namespace Soldank
{
class BackgroundRenderer
{
public:
    BackgroundRenderer(PMSColor background_top_color,
                       PMSColor background_bottom_color,
                       std::span<float, 4> boundaries);
    ~BackgroundRenderer();

    // it's not safe to be able to copy/move this because we would also need to take care of the
    // created OpenGL buffers and textures
    BackgroundRenderer(const BackgroundRenderer&) = delete;
    BackgroundRenderer& operator=(BackgroundRenderer other) = delete;
    BackgroundRenderer(BackgroundRenderer&&) = delete;
    BackgroundRenderer& operator=(BackgroundRenderer&& other) = delete;

    void Render(glm::mat4 transform);

private:
    Shader shader_;

    unsigned int vbo_;
};
} // namespace Soldank

#endif
