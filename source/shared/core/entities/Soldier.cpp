#include "core/entities/Soldier.hpp"

#include "core/entities/WeaponParametersFactory.hpp"

const float GRAV = 0.06F;

namespace Soldat
{
Soldier::Soldier(unsigned int soldier_id, glm::vec2 spawn_position)
    : id(soldier_id)
    , active(true)
    , dead_meat(false)
    , style(0)
    , num(1)
    , visible(1)
    , on_ground(false)
    , on_ground_for_law(false)
    , on_ground_last_frame(false)
    , on_ground_permanent(false)
    , direction(1)
    , old_direction(1)
    , health(150.0)
    , alpha(255.0)
    , jets_count(0)
    , jets_count_prev(0)
    , wear_helmet(0)
    , has_cigar(1)
    , vest(0.0)
    , idle_time(0)
    , idle_random(0)
    , position(0)
    , on_fire(0)
    , collider_distance(255)
    , half_dead(false)
    , skeleton(ParticleSystem::LoadFromFile("gostek.po", 4.5F, 1.0F, 1.06F * GRAV, 0.0F, 0.9945F))
    , legs_animation(AnimationType::Stand)
    , body_animation(AnimationType::Stand)
    , control()
    , active_weapon(0)
    , weapons{ { WeaponParametersFactory::GetParameters(WeaponType::DesertEagles, false) },
               { WeaponParametersFactory::GetParameters(WeaponType::Knife, false) },
               { WeaponParametersFactory::GetParameters(WeaponType::FragGrenade, false) } }
    , fired(0)
    , particle(true,
               spawn_position,
               spawn_position,
               glm::vec2(0.0F, 0.0F),
               glm::vec2(0.0F, 0.0F),
               1.0F,
               1.0F,
               GRAV,
               0.99,
               0.0F)
{
}
} // namespace Soldat
