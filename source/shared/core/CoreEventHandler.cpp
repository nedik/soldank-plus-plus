#include "core/CoreEventHandler.hpp"

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
}
} // namespace Soldank
