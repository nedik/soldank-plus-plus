#include "core/CoreEventHandler.hpp"

#include "core/math/Calc.hpp"

#include "core/entities/Bullet.hpp"
#include "core/entities/WeaponParametersFactory.hpp"

#include "core/physics/SoldierPhysics.hpp"

#include "spdlog/spdlog.h"
#include <utility>

namespace Soldank
{
void CoreEventHandler::ObserveAll(IWorld* world)
{
    ObserveAllWorldEvents(world);
    ObserveAllPhysicsEvents(world);
}

void CoreEventHandler::ObserveAllWorldEvents(IWorld* world)
{
    world->GetWorldEvents().after_soldier_spawns.AddObserver([](Soldier& soldier) {
        spdlog::debug("soldier {} spawned at ({}, {})",
                      soldier.id,
                      soldier.particle.position.x,
                      soldier.particle.position.y);
    });
}

void CoreEventHandler::ObserveAllPhysicsEvents(IWorld* world)
{
    world->GetPhysicsEvents().soldier_hit_by_bullet.AddObserver(
      [world](Soldier& soldier, float damage) {
          soldier.health -= damage;
          spdlog::debug("soldier {} hit by {} damage", soldier.id, damage);
          if (soldier.health <= 0.0F) {
              world->KillSoldier(soldier.id);
          }
      });
    world->GetPhysicsEvents().soldier_switches_weapon.AddObserver([world](Soldier& soldier) {
        spdlog::debug(
          "soldier {} switched weapon from {} to {}",
          soldier.id,
          std::to_underlying(soldier.weapons[soldier.active_weapon].GetWeaponParameters().kind),
          std::to_underlying(
            soldier.weapons[(soldier.active_weapon + 1) % 2].GetWeaponParameters().kind));
        world->GetStateManager()->SwitchSoldierWeapon(soldier.id);
    });
    world->GetPhysicsEvents().soldier_throws_knife.AddObserver([world](const Soldier& soldier) {
        spdlog::debug("soldier {} throws knife", soldier.id);
        Weapon weapon{ WeaponParametersFactory::GetParameters(WeaponType::ThrownKnife, false) };
        auto aim_x = (float)soldier.control.mouse_aim_x;
        auto aim_y = (float)soldier.control.mouse_aim_y;
        auto dir = Calc::Vec2Normalize(glm::vec2(aim_x, aim_y) - soldier.skeleton->GetPos(15));
        auto frame = (float)soldier.body_animation->GetFrame();
        auto thrown_mul = 1.5F * std::min(16.0F, std::max(8.0F, frame)) / 16.0F;
        auto bullet_vel = dir * weapon.GetWeaponParameters().speed * thrown_mul;
        auto inherited_vel =
          soldier.particle.GetVelocity() * weapon.GetWeaponParameters().inherited_velocity;
        auto velocity = bullet_vel + inherited_vel;

        BulletParams params{
            weapon.GetWeaponParameters().bullet_style,
            weapon.GetWeaponParameters().kind,
            soldier.skeleton->GetPos(16) + velocity,
            velocity,
            (std::int16_t)weapon.GetWeaponParameters().timeout,
            weapon.GetWeaponParameters().hit_multiply,
            TeamType::None,
            soldier.id,
            0.0F // TODO: Add push
        };
        world->GetStateManager()->CreateProjectile(params);
    });
    world->GetPhysicsEvents().soldier_fires_primary_weapon.AddObserver([world](Soldier& soldier) {
        std::vector<BulletParams> bullet_emitter;
        SoldierPhysics::Fire(soldier, bullet_emitter);
        for (auto& bullet_params : bullet_emitter) {
            world->GetStateManager()->CreateProjectile(bullet_params);
        }
    });
    world->GetPhysicsEvents().soldier_collides_with_item.AddObserver(
      [](Soldier& soldier, Item& item) {
          spdlog::info("COLLISION BETWEEN ITEM {} AND PLAYER {}", item.num, soldier.id);
      });
}
} // namespace Soldank
