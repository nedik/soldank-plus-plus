module;

#include <cstdint>

export module Shared.Core.Physics.Bullets.BulletImpactResolver;

import Extern.Glm;

import Shared.Core.Entities.Bullet;
import Shared.Core.Physics.Bullets.BulletDamage;
import Shared.Core.Physics.Bullets.BulletTypes;
import Shared.Core.Physics.PhysicsEvents;
import Shared.Core.State.StateManager;
import Shared.Core.Types.BulletType;
import Shared.Core.Types.ItemType;

namespace Soldank::BulletImpactResolver
{
namespace
{
constexpr float RICOCHET_REPEAT_DISTANCE = 50.0F;
constexpr float GRENADE_SURFACE_COEFFICIENT = 0.88F;
constexpr std::int16_t ARROW_RESIST_TIMEOUT = 280;

bool CanRicochet(const Bullet& bullet, const BulletCollisionResult& collision)
{
    return glm::length(collision.position - bullet.hit_spot) > RICOCHET_REPEAT_DISTANCE;
}

void Ricochet(Bullet& bullet, const BulletCollisionResult& collision)
{
    const glm::vec2 normal = *collision.surface_normal;
    const glm::vec2 velocity = bullet.particle.GetVelocity();

    bullet.hit_spot = collision.position;
    ++bullet.ricochet_count;
    bullet.particle.SetVelocity(velocity - 2.0F * glm::dot(velocity, normal) * normal);
    bullet.particle.position = collision.position + normal * 0.01F;
    bullet.particle.old_position = bullet.particle.position;
}

void Bounce(Bullet& bullet, const BulletCollisionResult& collision)
{
    const glm::vec2 normal = *collision.surface_normal;
    const glm::vec2 velocity = bullet.particle.GetVelocity();

    bullet.particle.SetVelocity((velocity - 2.0F * glm::dot(velocity, normal) * normal) *
                                GRENADE_SURFACE_COEFFICIENT);
    bullet.particle.position = collision.position + normal * 0.01F;
    bullet.particle.old_position = bullet.particle.position;
}

void StickArrow(Bullet& bullet, const BulletCollisionResult& collision)
{
    bullet.particle.position = collision.position - bullet.particle.GetVelocity();
    bullet.particle.old_position = bullet.particle.position;
    bullet.particle.SetForce(bullet.particle.GetForce() + glm::vec2{ 0.0F, -0.135F });
    if (bullet.timeout > ARROW_RESIST_TIMEOUT) {
        bullet.timeout = ARROW_RESIST_TIMEOUT;
    }
    if (bullet.timeout < 20) {
        bullet.particle.SetForce(bullet.particle.GetForce() + glm::vec2{ 0.0F, 0.135F });
    }
}
} // namespace
} // namespace Soldank::BulletImpactResolver

export namespace Soldank::BulletImpactResolver
{
void ResolveMapImpact(const PhysicsEvents& physics_events,
                      Bullet& bullet,
                      StateManager& state_manager,
                      const BulletCollisionResult& collision)
{
    switch (bullet.style) {
        case BulletType::Bullet:
        case BulletType::GaugeBullet:
        case BulletType::Fist:
        case BulletType::Blade:
        case BulletType::M2Bullet:
        case BulletType::M79Grenade:
        case BulletType::FlameArrow:
        case BulletType::LAWMissile:
            if (CanRicochet(bullet, collision)) {
                Ricochet(bullet, collision);
            } else {
                bullet.particle.position = collision.position;
                bullet.active = false;
            }
            break;
        case BulletType::FragGrenade:
        case BulletType::Flame:
            Bounce(bullet, collision);
            if (bullet.style == BulletType::Flame && bullet.timeout > 16) {
                bullet.timeout = 16;
            }
            break;
        case BulletType::Arrow:
            StickArrow(bullet, collision);
            break;
        case BulletType::ClusterGrenade:
        case BulletType::Cluster:
            bullet.particle.position = collision.position;
            bullet.active = false;
            break;
        case BulletType::ThrownKnife:
            bullet.particle.position = collision.position - bullet.particle.GetVelocity();
            state_manager.CreateItem(bullet.particle.position, 0, ItemType::Knife);
            bullet.active = false;
            break;
    }

    physics_events.bullet_collides_with_polygon.Notify(bullet, collision.position);
}

void ResolveBlockingImpact(Bullet& bullet, const BulletCollisionResult& collision)
{
    bullet.particle.position = collision.position;
    bullet.active = false;
}

void ResolveImpact(const PhysicsEvents& physics_events,
                   Bullet& bullet,
                   StateManager& state_manager,
                   const BulletCollisionResult& collision)
{
    switch (collision.kind) {
        case BulletCollisionKind::MapPolygon:
            ResolveMapImpact(physics_events, bullet, state_manager, collision);
            return;
        case BulletCollisionKind::Soldier:
            if (BulletDamage::ApplyDirectHit(physics_events, bullet, state_manager, collision) ==
                BulletDamage::DirectHitOutcome::Destroyed) {
                bullet.active = false;
            }
            return;
        case BulletCollisionKind::MapCollider:
        case BulletCollisionKind::Item:
            ResolveBlockingImpact(bullet, collision);
            return;
    }
}

void ResolveExpiry(Bullet& bullet)
{
    bullet.active = false;
}
} // namespace Soldank::BulletImpactResolver
