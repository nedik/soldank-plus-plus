#ifndef __CORE_EVENT_HANDLER_HPP__
#define __CORE_EVENT_HANDLER_HPP__

#include "core/WorldEvents.hpp"
#include "core/physics/PhysicsEvents.hpp"

namespace Soldank
{
class CoreEventHandler
{
public:
    static void ObserveAll(WorldEvents& world_events, PhysicsEvents& physics_events);

private:
    static void ObserveAllWorldEvents(WorldEvents& world_events);
    static void ObserveAllPhysicsEvents(PhysicsEvents& physics_events);
};
} // namespace Soldank

#endif
