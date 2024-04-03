#ifndef __SCENERIESRENDERER_HPP__
#define __SCENERIESRENDERER_HPP__

#include "rendering/shaders/Shader.hpp"

#include "core/map/Map.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace Soldank
{
class SceneriesRenderer
{
public:
    SceneriesRenderer(const std::vector<PMSSceneryType>& scenery_types,
                      const std::vector<PMSScenery>& scenery_instances);
    ~SceneriesRenderer();

    // it's not safe to be able to copy/move this because we would also need to take care of the
    // created OpenGL buffers and textures
    SceneriesRenderer(const SceneriesRenderer&) = delete;
    SceneriesRenderer& operator=(SceneriesRenderer other) = delete;
    SceneriesRenderer(SceneriesRenderer&&) = delete;
    SceneriesRenderer& operator=(SceneriesRenderer&& other) = delete;

    void Render(glm::mat4 transform,
                int target_level,
                const std::vector<PMSScenery>& scenery_instances);

private:
    Shader shader_;

    std::vector<unsigned int> textures_;
    unsigned int vbo_;
    unsigned int ebo_;
};
} // namespace Soldank

#endif
