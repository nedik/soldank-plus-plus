module;

#include <cstdint>

export module Shared.Core.Physics.Bullets.BulletImpactResolver;

import Extern.Glm;

import Shared.Core.Entities.Bullet;
import Shared.Core.Entities.Item;
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
constexpr float THING_PUSH_MULTIPLIER = 9.0F;
constexpr unsigned int THING_COLLISION_COOLDOWN = 60;

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

void ResolveItemImpact(Bullet& bullet,
                       StateManager& state_manager,
                       const BulletCollisionResult& collision)
{
    if (!collision.item_id.has_value() || !collision.item_particle_id.has_value()) {
        return;
    }

    const unsigned int game_tick = state_manager.GetGameTick();
    bool has_expired_cooldown = false;
    for (const auto& cooldown : bullet.item_collision_cooldowns) {
        if (cooldown.item_id != *collision.item_id) {
            continue;
        }

        if (game_tick < cooldown.cooldown_end_tick) {
            return;
        }

        has_expired_cooldown = true;
        break;
    }

    bool applied_push = false;
    state_manager.TransformItems([&](Item& item) {
        if (item.id != *collision.item_id || !item.skeleton ||
            *collision.item_particle_id > item.skeleton->GetParticles().size()) {
            return;
        }

        const unsigned int particle_id = *collision.item_particle_id;
        const glm::vec2 item_velocity =
          item.skeleton->GetPos(particle_id) - item.skeleton->GetOldPos(particle_id);
        const glm::vec2 push =
          (bullet.particle.GetVelocity() - item_velocity) * bullet.push * THING_PUSH_MULTIPLIER;
        item.skeleton->SetPos(particle_id, item.skeleton->GetPos(particle_id) + push);
        item.static_type = false;
        applied_push = true;
    });

    if (!applied_push) {
        return;
    }

    if (!has_expired_cooldown) {
        bullet.item_collision_cooldowns.push_back(
          { *collision.item_id, game_tick + THING_COLLISION_COOLDOWN });
        return;
    }

    for (auto& cooldown : bullet.item_collision_cooldowns) {
        if (cooldown.item_id == *collision.item_id) {
            cooldown.cooldown_end_tick = game_tick + THING_COLLISION_COOLDOWN;
            return;
        }
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
            ResolveBlockingImpact(bullet, collision);
            return;
        case BulletCollisionKind::Item:
            ResolveItemImpact(bullet, state_manager, collision);
            return;
    }
}

void ResolveExpiry(Bullet& bullet)
{
    bullet.active = false;
}
} // namespace Soldank::BulletImpactResolver
