#ifndef __PHYSICS_EVENTS_HPP__
#define __PHYSICS_EVENTS_HPP__

#include "core/map/PMSStructs.hpp"
#include "core/utility/Observable.hpp"
#include "core/entities/Soldier.hpp"

namespace Soldank
{
struct PhysicsEvents
{
    Observable<Soldier&, float /* damage */> soldier_hit_by_bullet;
    Observable<Soldier&> soldier_switches_weapon;
    Observable<const Soldier&> soldier_throws_knife;
    Observable<Soldier&> soldier_fires_primary_weapon;
    Observable<const Soldier&, const PMSPolygon&> soldier_collides_with_polygon;
};
} // namespace Soldank

#endif
