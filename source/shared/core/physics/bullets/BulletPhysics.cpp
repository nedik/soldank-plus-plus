module;

#include <algorithm>
#include <array>
#include <cmath>
#include <initializer_list>
#include <optional>

export module Shared.Core.Physics.BulletPhysics;

import Extern.Glm;

import Shared.Core.Entities.Bullet;
import Shared.Core.Physics.Bullets.BulletCollision;
import Shared.Core.Physics.Bullets.BulletDamage;
import Shared.Core.Physics.Bullets.BulletImpactResolver;
import Shared.Core.Physics.Bullets.BulletTypes;
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

std::optional<BulletCollisionResult> FindClosestCollision(
  std::initializer_list<std::optional<BulletCollisionResult>> collisions)
{
    std::optional<BulletCollisionResult> closest_collision;
    for (const auto& collision : collisions) {
        if (!collision.has_value() ||
            (closest_collision.has_value() && collision->distance >= closest_collision->distance)) {
            continue;
        }

        closest_collision = collision;
    }

    return closest_collision;
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

    const auto collision = FindClosestCollision({
      BulletCollision::FindMapCollision(bullet, map),
      BulletCollision::FindColliderCollision(bullet, map),
      BulletCollision::FindSoldierCollision(bullet, state_manager, -1.0F),
      BulletCollision::FindThingCollision(bullet, state_manager),
    });
    if (collision.has_value()) {
        switch (collision->kind) {
            case BulletCollisionKind::MapPolygon:
                BulletImpactResolver::ResolveMapImpact(physics_events, bullet, *collision);
                break;
            case BulletCollisionKind::Soldier:
                if (BulletDamage::ApplyDirectHit(
                      physics_events, bullet, state_manager, *collision) ==
                    BulletDamage::DirectHitOutcome::Destroyed) {
                    bullet.active = false;
                }
                break;
            case BulletCollisionKind::MapCollider:
            case BulletCollisionKind::Item:
                BulletImpactResolver::ResolveBlockingImpact(bullet, *collision);
                break;
        }
    }

    ApplyTimeoutAndDamageFalloff(bullet);

    if (IsOutOfBounds(bullet, map)) {
        bullet.active = false;
    }
}
} // namespace Soldank::BulletPhysics
