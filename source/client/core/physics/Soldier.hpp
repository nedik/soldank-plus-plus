#ifndef __SOLDIER_HPP__
#define __SOLDIER_HPP__

#include "core/animations/Animation.hpp"
#include "core/math/Glm.hpp"
#include "core/physics/Particles.hpp"
#include "core/state/Control.hpp"
#include "core/state/State.hpp"
#include "core/entities/Weapon.hpp"

#include <cstdint>
#include <vector>
#include <memory>

namespace Soldat
{
class Soldier
{
public:
    Soldier(glm::vec2 spawn_position);

    const Weapon& GetPrimaryWeapon() const { return weapons[active_weapon]; }

    const Weapon& GetSecondaryWeapon() const { return weapons[(active_weapon + 1) % 2]; }

    const Weapon& GetTertiaryWeapon() const { return weapons[2]; }

    void SwitchWeapon();
    void UpdateKeys(const Control& _control);
    void LegsApplyAnimation(AnimationType id, unsigned int frame);
    void BodyApplyAnimation(AnimationType id, unsigned int frame);
    void HandleSpecialPolytypes(const Map& map, PMSPolygonType polytype, glm::vec2 _pos);
    void UpdateControl(State& state);
    void Update(State& state);

    bool CheckMapCollision(const Map& map, float x, float y, int area, State& state);
    bool CheckMapVerticesCollision(const Map& map,
                                   float x,
                                   float y,
                                   float r,
                                   bool has_collided,
                                   State& state);
    bool CheckRadiusMapCollision(const Map& map, float x, float y, bool has_collided, State& state);
    bool CheckSkeletonMapCollision(const Map& map, unsigned int i, float x, float y, State& state);
    void AddCollidingPoly(State& state, unsigned int poly_id);
    void Fire();

    bool active;
    bool dead_meat;
    std::uint8_t style;
    unsigned int num;
    std::uint8_t visible;
    bool on_ground;
    bool on_ground_for_law;
    bool on_ground_last_frame;
    bool on_ground_permanent;
    std::int8_t direction;
    std::int8_t old_direction;
    float health;
    std::uint8_t alpha;
    std::int32_t jets_count;
    std::int32_t jets_count_prev;
    std::uint8_t wear_helmet;
    std::uint8_t has_cigar;
    float vest;
    std::int32_t idle_time;
    std::int8_t idle_random;
    std::uint8_t position;
    std::uint8_t on_fire;
    std::uint8_t collider_distance;
    bool half_dead;
    std::shared_ptr<ParticleSystem> skeleton;
    AnimationState legs_animation;
    AnimationState body_animation;
    Control control;
    unsigned int active_weapon;
    std::vector<Weapon> weapons;
    std::uint8_t fired;
    Particle particle;
};
} // namespace Soldat

#endif
