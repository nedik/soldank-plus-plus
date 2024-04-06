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

namespace Soldank
{
class SoldierPhysics
{
public:
    static const Weapon& GetPrimaryWeapon(Soldier& soldier)
    {
        return soldier.weapons[soldier.active_weapon];
    }

    static const Weapon& GetSecondaryWeapon(Soldier& soldier)
    {
        return soldier.weapons[(soldier.active_weapon + 1) % 2];
    }

    static const Weapon& GetTertiaryWeapon(Soldier& soldier) { return soldier.weapons[2]; }

    static void SwitchWeapon(Soldier& soldier);
    static void UpdateKeys(Soldier& soldier, const Control& control);
    static void LegsApplyAnimation(Soldier& soldier, AnimationType id, unsigned int frame);
    static void BodyApplyAnimation(Soldier& soldier, AnimationType id, unsigned int frame);
    static void HandleSpecialPolytypes(const Map& map, PMSPolygonType polytype, Soldier& soldier);
    static void UpdateControl(State& state,
                              Soldier& soldier,
                              std::vector<BulletParams>& bullet_emitter);
    static void Update(State& state, Soldier& soldier, std::vector<BulletParams>& bullet_emitter);

    static bool CheckMapCollision(Soldier& soldier,
                                  const Map& map,
                                  float x,
                                  float y,
                                  int area,
                                  State& state);
    static bool CheckMapVerticesCollision(Soldier& soldier,
                                          const Map& map,
                                          float x,
                                          float y,
                                          float r,
                                          bool has_collided,
                                          State& state);
    static bool CheckRadiusMapCollision(Soldier& soldier,
                                        const Map& map,
                                        float x,
                                        float y,
                                        bool has_collided,
                                        State& state);
    static bool CheckSkeletonMapCollision(Soldier& soldier,
                                          const Map& map,
                                          unsigned int i,
                                          float x,
                                          float y,
                                          State& state);
    static void Fire(Soldier& soldier, std::vector<BulletParams>& bullet_emitter);
};
} // namespace Soldank

#endif
