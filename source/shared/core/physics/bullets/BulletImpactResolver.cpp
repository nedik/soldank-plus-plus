module;

export module Shared.Core.Physics.Bullets.BulletImpactResolver;

import Shared.Core.Entities.Bullet;
import Shared.Core.Physics.Bullets.BulletDamage;
import Shared.Core.Physics.Bullets.BulletTypes;
import Shared.Core.Physics.PhysicsEvents;
import Shared.Core.State.StateManager;

export namespace Soldank::BulletImpactResolver
{
void ResolveMapImpact(const PhysicsEvents& physics_events,
                      Bullet& bullet,
                      const BulletCollisionResult& collision)
{
    bullet.particle.position = collision.position;
    bullet.active = false;
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
            ResolveMapImpact(physics_events, bullet, collision);
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
