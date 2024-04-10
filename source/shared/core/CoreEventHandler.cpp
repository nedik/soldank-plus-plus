#include "core/CoreEventHandler.hpp"

#include "spdlog/spdlog.h"

namespace Soldank
{
void CoreEventHandler::ObserveAll(WorldEvents& world_events, PhysicsEvents& physics_events)
{
    ObserveAllWorldEvents(world_events);
    ObserveAllPhysicsEvents(physics_events);
}

void CoreEventHandler::ObserveAllWorldEvents(WorldEvents& world_events)
{
    world_events.after_soldier_spawns.AddObserver([](Soldier& soldier) {
        spdlog::debug("soldier {} spawned at ({}, {})",
                      soldier.id,
                      soldier.particle.position.x,
                      soldier.particle.position.y);
    });
}

void CoreEventHandler::ObserveAllPhysicsEvents(PhysicsEvents& physics_events)
{
    physics_events.soldier_hit_by_bullet.AddObserver([](Soldier& soldier, float damage) {
        soldier.health -= damage;
        spdlog::debug("soldier {} hit by {} damage", soldier.id, damage);
    });
}
} // namespace Soldank
