#ifndef __BULLET_PHYSICS_HPP__
#define __BULLET_PHYSICS_HPP__

#include "core/entities/Bullet.hpp"
#include "core/map/Map.hpp"
#include "core/state/State.hpp"
#include "core/physics/PhysicsEvents.hpp"

#include <optional>

namespace Soldank::BulletPhysics
{
void UpdateBullet(const PhysicsEvents& physics_events,
                  Bullet& bullet,
                  const Map& map,
                  State& state);
} // namespace Soldank::BulletPhysics

#endif
