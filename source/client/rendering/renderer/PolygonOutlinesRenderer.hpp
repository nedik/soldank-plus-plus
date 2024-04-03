#ifndef __POLYGONOUTLINESRENDERER_HPP__
#define __POLYGONOUTLINESRENDERER_HPP__

#include "rendering/shaders/Shader.hpp"

#include "core/map/Map.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace Soldank
{
class PolygonOutlinesRenderer
{
public:
    PolygonOutlinesRenderer(const std::vector<PMSPolygon>& polygons);
    ~PolygonOutlinesRenderer();

    // it's not safe to be able to copy/move this because we would also need to take care of the
    // created OpenGL buffers and textures
    PolygonOutlinesRenderer(const PolygonOutlinesRenderer&) = delete;
    PolygonOutlinesRenderer& operator=(PolygonOutlinesRenderer other) = delete;
    PolygonOutlinesRenderer(PolygonOutlinesRenderer&&) = delete;
    PolygonOutlinesRenderer& operator=(PolygonOutlinesRenderer&& other) = delete;

    void Render(glm::mat4 transform, unsigned int polygon_id);

private:
    Shader shader_;
    unsigned int polygons_count_;

    unsigned int vbo_;
};
} // namespace Soldank

#endif
