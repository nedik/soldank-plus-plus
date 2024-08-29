#ifndef __ITEM_PHYSICS_HPP__
#define __ITEM_PHYSICS_HPP__

#include <vector>

namespace Soldank
{
struct Item;
class Map;
class State;
struct PhysicsEvents;
} // namespace Soldank

namespace Soldank::ItemPhysics
{
void Update(State& state, Item& item, const PhysicsEvents& physics_events);
bool CheckMapCollision(Item& item,
                       const Map& map,
                       float x,
                       float y,
                       int i,
                       State& state,
                       const PhysicsEvents& physics_events);
int CheckSoldierCollision(Item& item, State& state);
} // namespace Soldank::ItemPhysics

#endif
