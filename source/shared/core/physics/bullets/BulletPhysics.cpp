module;

#include <algorithm>
#include <array>
#include <cmath>

export module Shared.Core.Physics.BulletPhysics;

import Extern.Glm;

import Shared.Core.Entities.Bullet;
import Shared.Core.Physics.Bullets.BulletCollision;
import Shared.Core.Physics.Bullets.BulletDamage;
import Shared.Core.Physics.Bullets.BulletImpactResolver;
import Shared.Core.Physics.PhysicsEvents;
import Shared.Core.Map.Map;
import Shared.Core.State.StateManager;
import Shared.Core.Types.WeaponType;

namespace Soldank
{
namespace
{
void ApplyTimeoutAndDamageFalloff(Bullet& bullet)
{
    bullet.timeout_prev = bullet.timeout;
    --bullet.timeout;

    if (bullet.timeout == 0) {
        bullet.active = false;
        return;
    }

    if (bullet.degrade_count >= 2 || bullet.timeout % 6 != 0) {
        return;
    }

    constexpr auto EXCLUDED_WEAPONS =
      std::array{ WeaponType::Barrett, WeaponType::M79, WeaponType::Knife, WeaponType::LAW };
    if (std::ranges::contains(EXCLUDED_WEAPONS, bullet.weapon)) {
        return;
    }

    constexpr auto DEGRADE_DISTANCES_SQUARED = std::array{ 500.0F * 500.0F, 900.0F * 900.0F };
    const glm::vec2 delta = bullet.particle.position - bullet.initial_position;
    const float distance_squared = delta.x * delta.x + delta.y * delta.y;
    if (distance_squared > DEGRADE_DISTANCES_SQUARED.at(bullet.degrade_count)) {
        bullet.hit_multiply_prev = bullet.hit_multiply;
        bullet.hit_multiply *= 0.5F;
        ++bullet.degrade_count;
    }
}

bool IsOutOfBounds(const Bullet& bullet, const Map& map)
{
    const float bound = static_cast<float>(map.GetSectorsCount() * map.GetSectorsSize() - 10);
    return std::max(std::abs(bullet.particle.position.x), std::abs(bullet.particle.position.y)) >
           bound;
}
} // namespace
} // namespace Soldank

export namespace Soldank::BulletPhysics
{
void UpdateBullet(const PhysicsEvents& physics_events,
                  Bullet& bullet,
                  const Map& map,
                  StateManager& state_manager)
{
    bullet.velocity_prev = bullet.particle.velocity_;
    bullet.particle.Euler();

    if (const auto collision = BulletCollision::FindMapCollision(bullet, map);
        collision.has_value()) {
        BulletImpactResolver::ResolveMapImpact(physics_events, bullet, *collision);
    }

    ApplyTimeoutAndDamageFalloff(bullet);

    if (IsOutOfBounds(bullet, map)) {
        bullet.active = false;
    }

    if (const auto collision = BulletCollision::FindSoldierCollision(bullet, state_manager, -1.0F);
        collision.has_value()) {
        BulletDamage::ApplyDirectHit(physics_events, bullet, state_manager, *collision);
    }
}
} // namespace Soldank::BulletPhysics
