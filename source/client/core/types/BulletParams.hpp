#ifndef __BULLET_PARAMS_HPP__
#define __BULLET_PARAMS_HPP__

#include "core/math/Glm.hpp"
#include "core/types/BulletType.hpp"
#include "core/types/TeamType.hpp"
#include "core/types/WeaponType.hpp"

namespace Soldat
{
struct BulletParams {
    BulletType style;
    WeaponType weapon;
    glm::vec2 position;
    glm::vec2 velocity;
    std::int16_t timeout;
    float hit_multiply;
    TeamType team;
};
} // namespace Soldat

#endif