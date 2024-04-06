#ifndef __BULLET_PHYSICS_HPP__
#define __BULLET_PHYSICS_HPP__

#include "core/entities/Bullet.hpp"
#include "core/map/Map.hpp"
#include "core/state/State.hpp"
#include "core/physics/PhysicsEvents.hpp"

#include <optional>

namespace Soldank
{
class BulletPhysics
{
public:
    void UpdateBullet(const PhysicsEvents& physics_events,
                      Bullet& bullet,
                      const Map& map,
                      State& state);

private:
    std::optional<std::pair<glm::vec2, unsigned int>> CheckMapCollision(Bullet& bullet,
                                                                        const Map& map);
    bool CollidesWithPoly(const PMSPolygon& poly, TeamType team);
    std::optional<glm::vec2> CheckSoldierCollision(const PhysicsEvents& physics_events,
                                                   Bullet& bullet,
                                                   const Map& map,
                                                   State& state,
                                                   float lasthitdist);
    glm::vec2 GetSoldierCollisionPoint(Soldier& soldier);
};
} // namespace Soldank

#endif
