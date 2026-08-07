module;

#include <optional>
#include <utility>

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
enum class DirectHitOutcome
{
    Destroyed,
    Penetrated,
};

std::optional<float> GetPenetrationVelocityMultiplier(bool target_was_dead,
                                                      bool target_is_dead,
                                                      float projectile_speed,
                                                      float weapon_speed)
{
    if (target_was_dead) {
        return 0.9F;
    }

    if (target_is_dead || projectile_speed > 23.0F) {
        return 0.75F;
    }

    if (projectile_speed > 5.0F && weapon_speed > 0.0F && projectile_speed / weapon_speed >= 0.9F) {
        return 0.66F;
    }

    return std::nullopt;
}

DirectHitOutcome ApplyDirectHit(const PhysicsEvents& physics_events,
                                Bullet& bullet,
                                StateManager& state_manager,
                                const BulletCollisionResult& collision)
{
    const Soldier& soldier = state_manager.GetSoldier(*collision.soldier_id);
    glm::vec2 bullet_velocity = bullet.particle.GetVelocity();
    glm::vec2 normal = collision.position - soldier.skeleton->GetPos(*collision.body_part_id);
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
            if (*collision.body_part_id <= 4) {
                hitbox_modifier = weapon_parameters.modifier_legs;
            } else if (*collision.body_part_id <= 11) {
                hitbox_modifier = weapon_parameters.modifier_chest;
            }

            const float speed = Calc::Vec2Length(bullet_velocity);
            const bool was_dead = soldier.dead_meat;
            const float damage = speed * weapon_parameters.hit_multiply * hitbox_modifier;
            state_manager.TransformSoldier(*collision.soldier_id, [&](Soldier& target) {
                physics_events.soldier_hit_by_bullet.Notify(target, damage);
            });

            const bool is_dead_after_hit =
              state_manager.GetSoldier(*collision.soldier_id).dead_meat;
            const auto penetration_multiplier = GetPenetrationVelocityMultiplier(
              was_dead, is_dead_after_hit, speed, weapon_parameters.speed);
            if (penetration_multiplier.has_value()) {
                bullet.particle.SetVelocity(
                  Calc::Vec2Scale(bullet_velocity, *penetration_multiplier));
                return DirectHitOutcome::Penetrated;
            }

            return DirectHitOutcome::Destroyed;
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
            return DirectHitOutcome::Destroyed;
    }

    std::unreachable();
}
} // namespace Soldank::BulletDamage
