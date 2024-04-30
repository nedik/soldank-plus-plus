#include <utility>

#include "core/entities/Soldier.hpp"

#include "core/entities/WeaponParametersFactory.hpp"
#include "core/physics/Particles.hpp"
#include "core/types/WeaponType.hpp"

const float GRAV = 0.06F;

namespace Soldank
{
Soldier::Soldier(std::uint8_t soldier_id,
                 glm::vec2 spawn_position,
                 const AnimationDataManager& animation_data_manager,
                 std::shared_ptr<ParticleSystem> skeleton)
    : id(soldier_id)
    , active(false)
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
    , stance(0)
    , on_fire(0)
    , collider_distance(255)
    , half_dead(false)
    , skeleton(std::move(skeleton))
    , legs_animation(animation_data_manager.Get(AnimationType::Stand))
    , body_animation(animation_data_manager.Get(AnimationType::Stand))
    , control()
    , active_weapon(0)
    , weapons{ { WeaponParametersFactory::GetParameters(WeaponType::DesertEagles, false) },
               { WeaponParametersFactory::GetParameters(WeaponType::Knife, false) },
               { WeaponParametersFactory::GetParameters(WeaponType::FragGrenade, false) } }
    , weapon_choices{ WeaponType::DesertEagles, WeaponType::Knife }
    , fired(0)
    , particle(false,
               spawn_position,
               spawn_position,
               glm::vec2(0.0F, 0.0F),
               glm::vec2(0.0F, 0.0F),
               1.0F,
               1.0F,
               GRAV,
               0.99,
               0.0F)
    , is_shooting(false)
{
}
} // namespace Soldank
