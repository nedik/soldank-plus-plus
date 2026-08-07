module;

export module Shared.Core.Physics.Bullets.BulletDamage;

import Extern.Glm;

import Shared.Core.Entities.Bullet;
import Shared.Core.Entities.Soldier;
import Shared.Core.Entities.WeaponParametersFactory;
import Shared.Core.Math.Calc;
import Shared.Core.Physics.Bullets.BulletTypes;
import Shared.Core.Physics.PhysicsEvents;
import Shared.Core.State.StateManager;
import Shared.Core.Types.BulletType;

export namespace Soldank::BulletDamage
{
void ApplyDirectHit(const PhysicsEvents& physics_events,
                    Bullet& bullet,
                    StateManager& state_manager,
                    const BulletSoldierCollision& collision)
{
    const Soldier& soldier = state_manager.GetSoldier(collision.soldier_id);
    glm::vec2 bullet_velocity = bullet.particle.GetVelocity();
    glm::vec2 normal = collision.position - soldier.skeleton->GetPos(collision.body_part_id);
    normal = Calc::Vec2Scale(normal, 1.3F);
    normal.y = -normal.y;

    if (!soldier.dead_meat && bullet.style != BulletType::FragGrenade &&
        bullet.style != BulletType::Flame && bullet.style != BulletType::Arrow) {
        const glm::vec2 bullet_push = Calc::Vec2Scale(bullet_velocity, bullet.push);
        (void)bullet_push;
        // TODO: schedule the latency-compensated projectile push.
    }

    switch (bullet.style) {
        case BulletType::Bullet:
        case BulletType::GaugeBullet:
        case BulletType::Fist:
        case BulletType::Blade:
        case BulletType::M2Bullet: {
            const auto weapon_parameters =
              WeaponParametersFactory::GetParameters(bullet.weapon, false /* TODO realistic */);
            float hitbox_modifier = weapon_parameters.modifier_head;
            if (collision.body_part_id <= 4) {
                hitbox_modifier = weapon_parameters.modifier_legs;
            } else if (collision.body_part_id <= 11) {
                hitbox_modifier = weapon_parameters.modifier_chest;
            }

            const float speed = Calc::Vec2Length(bullet_velocity);
            const bool was_dead = soldier.dead_meat;
            const float damage = speed * weapon_parameters.hit_multiply * hitbox_modifier;
            state_manager.TransformSoldier(collision.soldier_id, [&](Soldier& target) {
                physics_events.soldier_hit_by_bullet.Notify(target, damage);
            });

            if (was_dead) {
                bullet.particle.SetVelocity(Calc::Vec2Scale(bullet_velocity, 0.9F));
                bullet_velocity = bullet.particle.GetVelocity();
            }

            if (soldier.dead_meat || speed > 23.0F) {
                bullet.particle.SetVelocity(Calc::Vec2Scale(bullet_velocity, 0.75F));
            }

            bullet.active = false;
            break;
        }
        case BulletType::FragGrenade:
        case BulletType::Arrow:
        case BulletType::M79Grenade:
        case BulletType::FlameArrow:
        case BulletType::LAWMissile:
        case BulletType::Flame:
        case BulletType::Cluster:
        case BulletType::ClusterGrenade:
        case BulletType::ThrownKnife:
            // TODO: implement style-specific direct-hit damage and impact behavior.
            break;
    }
}
} // namespace Soldank::BulletDamage
