#ifndef __BULLET_HPP__
#define __BULLET_HPP__

#include "core/map/Map.hpp"
#include "core/math/Glm.hpp"
#include "core/physics/Particles.hpp"
#include "core/types/BulletType.hpp"
#include "core/types/TeamType.hpp"
#include "core/types/WeaponType.hpp"
#include "core/types/BulletParams.hpp"

#include <cstdint>
#include <optional>

namespace Soldat
{
class Bullet
{
public:
    Bullet(BulletParams params);

    void Update(const Map& map);
    std::optional<std::pair<glm::vec2, unsigned int>> CheckMapCollision(const Map& map);
    bool CollidesWithPoly(TeamType team);

    float sign(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3);
    bool PointInPoly(glm::vec2 pt, const PMSPolygon& poly);

    // TODO: Move this to private
    bool active = true;
    BulletType style;
    WeaponType weapon;
    TeamType team;
    Particle particle;
    glm::vec2 initial_position;
    glm::vec2 velocity_prev;
    std::int16_t timeout;
    std::int16_t timeout_prev;
    float timeout_real;
    float hit_multiply;
    float hit_multiply_prev;
    unsigned int degrade_count = 0;
    // pub sprite: Option<gfx::Weapon>,
};
} // namespace Soldat

#endif
