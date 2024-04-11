#ifndef __CORE_EVENT_HANDLER_HPP__
#define __CORE_EVENT_HANDLER_HPP__

#include "core/WorldEvents.hpp"
#include "core/physics/PhysicsEvents.hpp"
#include "core/IWorld.hpp"

namespace Soldank
{
class CoreEventHandler
{
public:
    static void ObserveAll(IWorld* world);

private:
    static void ObserveAllWorldEvents(IWorld* world);
    static void ObserveAllPhysicsEvents(IWorld* world);
};
} // namespace Soldank

#endif
