#ifndef __SOLDIER_PHYSICS_HPP__
#define __SOLDIER_PHYSICS_HPP__

#include "core/animations/Animation.hpp"
#include "core/math/Glm.hpp"
#include "core/physics/Particles.hpp"
#include "core/state/Control.hpp"
#include "core/state/State.hpp"
#include "core/entities/Weapon.hpp"
#include "core/entities/Bullet.hpp"
#include "core/entities/Soldier.hpp"
#include "core/map/Map.hpp"

#include <cstdint>
#include <vector>
#include <memory>

namespace Soldat
{
class SoldierPhysics
{
public:
    SoldierPhysics();

    static const Weapon& GetPrimaryWeapon(Soldier& soldier)
    {
        return soldier.weapons[soldier.active_weapon];
    }

    static const Weapon& GetSecondaryWeapon(Soldier& soldier)
    {
        return soldier.weapons[(soldier.active_weapon + 1) % 2];
    }

    static const Weapon& GetTertiaryWeapon(Soldier& soldier) { return soldier.weapons[2]; }

    void SwitchWeapon(Soldier& soldier);
    void UpdateKeys(Soldier& soldier, const Control& control);
    void LegsApplyAnimation(Soldier& soldier, AnimationType id, unsigned int frame);
    void BodyApplyAnimation(Soldier& soldier, AnimationType id, unsigned int frame);
    void HandleSpecialPolytypes(const Map& map, PMSPolygonType polytype, Soldier& soldier);
    void UpdateControl(State& state, Soldier& soldier, std::vector<BulletParams>& bullet_emitter);
    void Update(State& state, Soldier& soldier, std::vector<BulletParams>& bullet_emitter);

    bool CheckMapCollision(Soldier& soldier,
                           const Map& map,
                           float x,
                           float y,
                           int area,
                           State& state);
    bool CheckMapVerticesCollision(Soldier& soldier,
                                   const Map& map,
                                   float x,
                                   float y,
                                   float r,
                                   bool has_collided,
                                   State& state);
    bool CheckRadiusMapCollision(Soldier& soldier,
                                 const Map& map,
                                 float x,
                                 float y,
                                 bool has_collided,
                                 State& state);
    bool CheckSkeletonMapCollision(Soldier& soldier,
                                   const Map& map,
                                   unsigned int i,
                                   float x,
                                   float y,
                                   State& state);
    void Fire(Soldier& soldier, std::vector<BulletParams>& bullet_emitter);
};
} // namespace Soldat

#endif
