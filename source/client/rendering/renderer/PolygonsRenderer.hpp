#ifndef __POLYGONSRENDERER_HPP__
#define __POLYGONSRENDERER_HPP__

#include "rendering/shaders/Shader.hpp"

#include "core/map/Map.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace Soldat
{
class PolygonsRenderer
{
public:
    PolygonsRenderer(const std::vector<PMSPolygon>& polygons, const std::string& texture_name);
    ~PolygonsRenderer();

    // it's not safe to be able to copy/move this because we would also need to take care of the
    // created OpenGL buffers and textures
    PolygonsRenderer(const PolygonsRenderer&) = delete;
    PolygonsRenderer& operator=(PolygonsRenderer other) = delete;
    PolygonsRenderer(PolygonsRenderer&&) = delete;
    PolygonsRenderer& operator=(PolygonsRenderer&& other) = delete;

    void Render(glm::mat4 transform);

private:
    Shader shader_;
    unsigned int polygons_count_;

    unsigned int texture_;
    unsigned int vbo_;
};
} // namespace Soldat

#endif
