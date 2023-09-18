#include "Bullet.hpp"

namespace Soldat
{
Bullet::Bullet(BulletParams params)
    : style(params.style)
    , weapon(params.weapon)
    , team(params.team)
    , particle(true,
               params.position,
               params.velocity,
               params.velocity,
               glm::vec2(0.0F, 0.0F),
               1.0F,
               1.0F,
               0.06 * 2.25,
               0.99,
               0.0F)
    , initial_position(params.position)
    , velocity_prev(params.velocity)
    , timeout(params.timeout)
    , timeout_prev(params.timeout)
    , timeout_real((float)params.timeout)
    , hit_multiply(params.hit_multiply)
    , hit_multiply_prev(params.hit_multiply)
{
}

} // namespace Soldat
