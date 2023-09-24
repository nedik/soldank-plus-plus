#ifndef __STATE_HPP__
#define __STATE_HPP__

#include "core/map/Map.hpp"
#include "core/math/Glm.hpp"

#include <vector>

namespace Soldat
{
struct State
{
    State(const std::string& map_name);

    Map map;
    glm::vec2 camera;
    glm::vec2 camera_prev;
    glm::vec2 mouse;
    float game_width;
    float game_height;
    float gravity;
    std::vector<unsigned int> colliding_polygon_ids;
};
} // namespace Soldat

#endif
