#ifndef __BULLET_PHYSICS_HPP__
#define __BULLET_PHYSICS_HPP__

#include "core/entities/Bullet.hpp"
#include "core/map/Map.hpp"

#include <optional>

namespace Soldank
{
class BulletPhysics
{
public:
    void UpdateBullet(Bullet& bullet, const Map& map);

private:
    std::optional<std::pair<glm::vec2, unsigned int>> CheckMapCollision(Bullet& bullet,
                                                                        const Map& map);
    bool CollidesWithPoly(const PMSPolygon& poly, TeamType team);
};
} // namespace Soldank

#endif
