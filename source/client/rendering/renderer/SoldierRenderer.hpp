#ifndef __SOLDIERRENDERER_HPP__
#define __SOLDIERRENDERER_HPP__

#include "rendering/shaders/Shader.hpp"
#include "rendering/data/Sprites.hpp"

#include "core/map/Map.hpp"
#include "core/physics/Soldier.hpp"

#include "core/math/Glm.hpp"

#include <vector>
#include <optional>

namespace Soldat
{
class SoldierRenderer
{
public:
    SoldierRenderer();
    ~SoldierRenderer();

    // it's not safe to be able to copy/move this because we would also need to take care of the
    // created OpenGL buffers and textures
    SoldierRenderer(const SoldierRenderer&) = delete;
    SoldierRenderer& operator=(SoldierRenderer other) = delete;
    SoldierRenderer(SoldierRenderer&&) = delete;
    SoldierRenderer& operator=(SoldierRenderer&& other) = delete;

    static void GenerateVertices(std::vector<float>& vertices,
                                 const Sprites::SoldierPartData& part_data,
                                 bool flipped);

    void Render(glm::mat4 transform, const Soldier& soldier, double frame_percent);

private:
    Shader shader_;

    std::vector<std::pair<unsigned int, std::optional<unsigned int>>> vbos_;
    std::vector<unsigned int> ebos_;
    std::vector<Sprites::SoldierPartType> part_types_;
};
} // namespace Soldat

#endif
