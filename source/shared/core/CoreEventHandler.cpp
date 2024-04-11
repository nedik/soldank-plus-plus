#include "core/CoreEventHandler.hpp"

#include "spdlog/spdlog.h"

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
}
} // namespace Soldank
