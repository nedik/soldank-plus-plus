module;

#include <array>
#include <cmath>
#include <limits>
#include <optional>

export module Shared.Core.Physics.Bullets.BulletCollision;

import Extern.Glm;

import Shared.Core.Entities.Bullet;
import Shared.Core.Entities.Soldier;
import Shared.Core.Map.Map;
import Shared.Core.Map.PMSEnums;
import Shared.Core.Map.PMSStructs;
import Shared.Core.Math.Calc;
import Shared.Core.Physics.Bullets.BulletTypes;
import Shared.Core.State.StateManager;
import Shared.Core.Types.BulletType;
import Shared.Core.Types.TeamType;

namespace Soldank
{
namespace
{
bool CollidesWithPoly(const PMSPolygon& poly, TeamType team)
{
    switch (poly.polygon_type) {
        case PMSPolygonType::AlphaBullets:
            return team == TeamType::Alpha;
        case PMSPolygonType::BravoBullets:
            return team == TeamType::Bravo;
        case PMSPolygonType::CharlieBullets:
            return team == TeamType::Charlie;
        case PMSPolygonType::DeltaBullets:
            return team == TeamType::Delta;
        case PMSPolygonType::AlphaPlayers:
        case PMSPolygonType::BravoPlayers:
        case PMSPolygonType::CharliePlayers:
        case PMSPolygonType::DeltaPlayers:
        case PMSPolygonType::OnlyPlayersCollide:
        case PMSPolygonType::NoCollide:
        case PMSPolygonType::FlaggerCollides:
        case PMSPolygonType::NonFlaggerCollides:
            // TODO: missing polygon types
            // case PMSPolygonType::ptBACKGROUND:
            // case PMSPolygonType::ptBACKGROUND_TRANSITION:
            return false;
        default:
            return true;
    }
}

glm::vec2 GetSoldierCollisionPoint(const Soldier& soldier)
{
    // TODO: use lag-compensated historical positions.
    return soldier.particle.position;
}

std::optional<BulletCollisionResult> FindSoldierCollisionPoint(const Soldier& soldier,
                                                               const Bullet& bullet)
{
    constexpr auto BODY_PARTS_PRIORITY = std::array{ 12, 11, 10, 6, 5, 4, 3 };
    constexpr int PART_RADIUS = 7;

    if (bullet.style == BulletType::Fist || bullet.style == BulletType::Blade) {
        // TODO: implement melee collision from the owner's hand direction.
        return std::nullopt;
    }

    const int radius = bullet.style == BulletType::FragGrenade ? PART_RADIUS + 1 : PART_RADIUS;
    const glm::vec2 collision_origin = GetSoldierCollisionPoint(soldier);
    const glm::vec2 start_point = bullet.particle.position;
    const glm::vec2 end_point = start_point + bullet.particle.GetVelocity();
    float min_distance = std::numeric_limits<float>::max();
    std::optional<BulletCollisionResult> closest_collision;

    for (const int body_part_id : BODY_PARTS_PRIORITY) {
        glm::vec2 body_part_offset =
          soldier.skeleton->GetPos(body_part_id) - soldier.particle.position;
        glm::vec2 collision_position = collision_origin + body_part_offset;
        collision_position.x -= 2.0F;

        const auto hit_position =
          Calc::LineCircleCollision(start_point, end_point, collision_position, radius);
        if (!hit_position.has_value()) {
            continue;
        }

        const float distance = Calc::Vec2Length(*hit_position - bullet.particle.old_position);
        if (distance < min_distance) {
            min_distance = distance;
            closest_collision = BulletCollisionResult{
                .kind = BulletCollisionKind::Soldier,
                .position = *hit_position,
                .distance = distance,
                .soldier_id = soldier.id,
                .body_part_id = body_part_id,
            };
        }
    }

    return closest_collision;
}
} // namespace
} // namespace Soldank

export namespace Soldank::BulletCollision
{
std::optional<BulletCollisionResult> FindMapCollision(const Bullet& bullet, const Map& map)
{
    const glm::vec2 start_point = bullet.particle.old_position;
    const glm::vec2 end_point = bullet.particle.position;
    const float steps = std::ceil(glm::length(end_point - start_point) / 2.5F);

    for (float i = 0.0F; i <= steps; ++i) {
        const glm::vec2 position = Calc::Lerp(start_point, end_point, i / steps);
        const glm::ivec2 sector_index = map.GetSectorIndex(position);
        const int sector_x = sector_index.x;
        const int sector_y = sector_index.y;

        if (sector_x <= 0 || sector_x >= map.GetSectorsCount() + 25 || sector_y <= 0 ||
            sector_y >= map.GetSectorsCount() + 25) {
            continue;
        }

        for (const unsigned int polygon_reference : map.GetSector(sector_x, sector_y).polygons) {
            const unsigned int polygon_id = polygon_reference - 1;
            const PMSPolygon& polygon = map.GetPolygons()[polygon_id];
            if (CollidesWithPoly(polygon, bullet.team) && Map::PointInPoly(position, polygon)) {
                return BulletCollisionResult{
                    .kind = BulletCollisionKind::MapPolygon,
                    .position = position,
                    .distance = Calc::Vec2Length(position - start_point),
                    .polygon_id = polygon_id,
                };
            }
        }
    }

    return std::nullopt;
}

std::optional<BulletCollisionResult> FindSoldierCollision(const Bullet& bullet,
                                                          const StateManager& state_manager,
                                                          float last_hit_distance)
{
    constexpr int ARROW_RESIST = 280;
    if (bullet.style == BulletType::Arrow && bullet.timeout <= ARROW_RESIST) {
        return std::nullopt;
    }

    const Soldier* soldier = state_manager.FindSoldier([&](const Soldier& candidate) {
        return FindSoldierCollisionPoint(candidate, bullet).has_value();
    });
    if (soldier == nullptr ||
        ((bullet.style == BulletType::Fist || bullet.style == BulletType::Blade) &&
         soldier->id == bullet.owner_id)) {
        return std::nullopt;
    }

    const auto collision = FindSoldierCollisionPoint(*soldier, bullet);
    if (!collision.has_value()) {
        return std::nullopt;
    }

    if (last_hit_distance > -1.0F) {
        if (collision->distance > last_hit_distance) {
            return std::nullopt;
        }
    }

    return collision;
}
} // namespace Soldank::BulletCollision
