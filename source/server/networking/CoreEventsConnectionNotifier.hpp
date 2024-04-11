#ifndef __CORE_EVENTS_CONNECTION_NOTIFIER_HPP__
#define __CORE_EVENTS_CONNECTION_NOTIFIER_HPP__

#include "networking/IGameServer.hpp"

#include "core/WorldEvents.hpp"
#include "core/physics/PhysicsEvents.hpp"

namespace Soldank
{
class CoreEventsConnectionNotifier
{
public:
    static void ObserveAll(IGameServer* game_server,
                           WorldEvents& world_events,
                           PhysicsEvents& physics_events);

private:
    static void ObserveAllWorldEvents(IGameServer* game_server, WorldEvents& world_events);
    static void ObserveAllPhysicsEvents(IGameServer* game_server, PhysicsEvents& physics_events);
};
} // namespace Soldank

#endif
