#ifndef __STATE_HPP__
#define __STATE_HPP__

#include "core/map/Map.hpp"
#include "core/entities/Bullet.hpp"
#include "core/entities/Soldier.hpp"

#include <vector>
#include <list>

namespace Soldat
{
struct State
{
    State(const std::string& map_name);

    unsigned int game_tick;
    Map map;
    float gravity;
    std::list<Bullet> bullets;
    std::list<Soldier> soldiers;
    std::vector<unsigned int> colliding_polygon_ids;
};
} // namespace Soldat

#endif
