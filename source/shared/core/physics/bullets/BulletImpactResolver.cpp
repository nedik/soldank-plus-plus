module;

export module Shared.Core.Physics.Bullets.BulletImpactResolver;

import Shared.Core.Entities.Bullet;
import Shared.Core.Physics.Bullets.BulletTypes;
import Shared.Core.Physics.PhysicsEvents;

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
} // namespace Soldank::BulletImpactResolver
