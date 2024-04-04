#ifndef __BULLET_HPP__
#define __BULLET_HPP__

#include "core/physics/Particles.hpp"
#include "core/math/Glm.hpp"
#include "core/types/BulletType.hpp"
#include "core/types/TeamType.hpp"
#include "core/types/WeaponType.hpp"

namespace Soldank
{
struct BulletParams
{
    BulletType style;
    WeaponType weapon;
    glm::vec2 position;
    glm::vec2 velocity;
    std::int16_t timeout;
    float hit_multiply;
    TeamType team;
    unsigned int owner_id;
    float push;
};

struct Bullet
{
    Bullet(BulletParams bullet_params);

    bool active = true;
    BulletType style;
    WeaponType weapon;
    TeamType team;
    unsigned int owner_id;
    Particle particle;
    glm::vec2 initial_position;
    glm::vec2 velocity_prev;
    std::int16_t timeout;
    std::int16_t timeout_prev;
    float timeout_real;
    float hit_multiply;
    float hit_multiply_prev;
    unsigned int degrade_count = 0;
    float push;
};
} // namespace Soldank

#endif
