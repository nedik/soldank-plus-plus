#include "BulletPhysics.hpp"

#include "core/map/PMSEnums.hpp"
#include "core/map/PMSStructs.hpp"
#include "core/math/Calc.hpp"
#include "core/types/TeamType.hpp"

#include <algorithm>
#include <array>

namespace Soldat
{
void BulletPhysics::UpdateBullet(Bullet& bullet, const Map& map)
{
    bullet.velocity_prev = bullet.particle.velocity_;
    bullet.particle.Euler();

    auto collision = CheckMapCollision(bullet, map);
    if (collision) {
        bullet.particle.position = collision.value().first;
        bullet.active = false;
    }

    bullet.timeout_prev = bullet.timeout;
    bullet.timeout -= 1;

    if (bullet.timeout == 0) {
        bullet.active = false;
    } else if (bullet.degrade_count < 2 && bullet.timeout % 6 == 0) {
        constexpr auto EXCEPT =
          std::array{ WeaponType::Barrett, WeaponType::M79, WeaponType::Knife, WeaponType::LAW };

        if (!std::ranges::contains(EXCEPT, bullet.weapon)) {
            // let dist2 = (self.particle.pos - self.initial_pos).magnitude2();
            auto delta = bullet.particle.position - bullet.initial_position;
            auto dist2 = delta.x * delta.x + delta.y * delta.y;
            auto degrade_dists2 = std::array{ 500.0F * 500.0F, 900.0F * 900.0F };

            if (dist2 > degrade_dists2.at(bullet.degrade_count)) {
                bullet.hit_multiply_prev = bullet.hit_multiply;
                bullet.hit_multiply *= 0.5;
                bullet.degrade_count += 1;
            }
        }
    }

    float x = bullet.particle.position.x;
    float y = bullet.particle.position.y;
    if (std::max(std::abs(x), std::abs(y)) >
        (float)(map.GetSectorsCount() * map.GetSectorsSize() - 10)) {
        bullet.active = false;
    }
}

std::optional<std::pair<glm::vec2, unsigned int>> BulletPhysics::CheckMapCollision(Bullet& bullet,
                                                                                   const Map& map)
{
    auto a = bullet.particle.old_position;
    auto b = bullet.particle.position;

    auto delta = b - a;
    float steps = std::ceil(glm::length(delta) / 2.5F);

    for (float i = 0; i <= steps; i++) {
        auto xy = Calc::Lerp(a, b, i / steps);

        // TODO: Take from sectors
        for (int poly_id = 0; poly_id < map.GetPolygons().size(); poly_id++) {
            const PMSPolygon& poly = map.GetPolygons()[poly_id];
            if (CollidesWithPoly(poly, bullet.team) &&
                map.PointInPoly(glm::vec2(xy.x, xy.y), poly)) {
                return std::make_pair(xy, poly_id);
            }
        }
    }

    return std::nullopt;
}

bool BulletPhysics::CollidesWithPoly(const PMSPolygon& poly, TeamType team)
{
    switch (poly.polygonType) {
        case PMSPolygonType::ptALPHABULLETS:
            return team == TeamType::Alpha;
        case PMSPolygonType::ptBRAVOBULLETS:
            return team == TeamType::Bravo;
        case PMSPolygonType::ptCHARLIEBULLETS:
            return team == TeamType::Charlie;
        case PMSPolygonType::ptDELTABULLETS:
            return team == TeamType::Delta;
        case PMSPolygonType::ptALPHAPLAYERS:
        case PMSPolygonType::ptBRAVOPLAYERS:
        case PMSPolygonType::ptCHARLIEPLAYERS:
        case PMSPolygonType::ptDELTAPLAYERS:
        case PMSPolygonType::ptONLY_PLAYERS_COLLIDE:
        case PMSPolygonType::ptNO_COLLIDE:
        case PMSPolygonType::ptFLAGGERCOLLIDES:
        case PMSPolygonType::ptNONFLAGGERCOLLIDES:
            // TODO: missing polygon types
            // case PMSPolygonType::ptBACKGROUND:
            // case PMSPolygonType::ptBACKGROUND_TRANSITION:
            return false;
        default:
            return true;
    }
}
} // namespace Soldat
