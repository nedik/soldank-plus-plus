#include "Bullet.hpp"

#include "core/math/Calc.hpp"

#include <algorithm>
#include <array>

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
//, sprite(params.sprite)
{
}

void Bullet::Update(const Map& map)
{
    velocity_prev = particle.velocity_;
    particle.Euler();

    auto collision = CheckMapCollision(map);
    if (collision) {
        particle.position = collision.value().first;
        active = false;
    }

    timeout_prev = timeout;
    timeout -= 1;

    if (timeout == 0) {
        active = false;
    } else if (degrade_count < 2 && timeout % 6 == 0) {
        constexpr auto EXCEPT =
          std::array{ WeaponType::Barrett, WeaponType::M79, WeaponType::Knife, WeaponType::LAW };

        if (!std::ranges::contains(EXCEPT, weapon)) {
            // let dist2 = (self.particle.pos - self.initial_pos).magnitude2();
            auto delta = particle.position - initial_position;
            auto dist2 = delta.x * delta.x + delta.y * delta.y;
            auto degrade_dists2 = std::array{ 500.0F * 500.0F, 900.0F * 900.0F };

            if (dist2 > degrade_dists2.at(degrade_count)) {
                hit_multiply_prev = hit_multiply;
                hit_multiply *= 0.5;
                degrade_count += 1;
            }
        }
    }

    float x = particle.position.x;
    float y = particle.position.y;
    if (std::max(std::abs(x), std::abs(y)) >
        (float)(map.GetSectorsCount() * map.GetSectorsSize() - 10)) {
        active = false;
    }
}

std::optional<std::pair<glm::vec2, unsigned int>> Bullet::CheckMapCollision(const Map& map)
{
    auto a = particle.old_position;
    auto b = particle.position;

    auto delta = b - a;
    float steps = std::ceil(glm::length(delta) / 2.5F);

    for (float i = 0; i <= steps; i++) {
        auto xy = Calc::Lerp(a, b, i / steps);

        // TODO: Take from sectors
        for (int poly_id = 0; poly_id < map.GetPolygons().size(); poly_id++) {
            const PMSPolygon& poly = map.GetPolygons()[poly_id];
            if (CollidesWithPoly(/*poly, */ team) && PointInPoly(glm::vec2(xy.x, xy.y), poly)) {
                return std::make_pair(xy, poly_id);
            }
        }
    }

    return std::nullopt;
}

bool Bullet::CollidesWithPoly(TeamType /*team*/)
{
    // TODO
    /*match self.polytype {
        PolyType::AlphaBullets => team == Team::Alpha,
        PolyType::BravoBullets => team == Team::Bravo,
        PolyType::CharlieBullets => team == Team::Charlie,
        PolyType::DeltaBullets => team == Team::Delta,
        PolyType::AlphaPlayers => false,
        PolyType::BravoPlayers => false,
        PolyType::CharliePlayers => false,
        PolyType::DeltaPlayers => false,
        PolyType::OnlyPlayersCollide => false,
        PolyType::NoCollide => false,
        PolyType::OnlyFlaggers => false,
        PolyType::NotFlaggers => false,
        PolyType::Background => false,
        PolyType::BackgroundTransition => false,
        _ => true,
    }*/
    return true;
}

float Bullet::sign(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool Bullet::PointInPoly(glm::vec2 pt, const PMSPolygon& poly)
{
    glm::vec2 v1(poly.vertices[0].x, poly.vertices[0].y);
    glm::vec2 v2(poly.vertices[1].x, poly.vertices[1].y);
    glm::vec2 v3(poly.vertices[2].x, poly.vertices[2].y);

    float d1 = NAN;
    float d2 = NAN;
    float d3 = NAN;
    bool has_neg = false;
    bool has_pos = false;

    d1 = sign(pt, v1, v2);
    d2 = sign(pt, v2, v3);
    d3 = sign(pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}
} // namespace Soldat
