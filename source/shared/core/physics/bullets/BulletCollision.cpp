module;

#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>

export module Shared.Core.Physics.Bullets.BulletCollision;

import Extern.Glm;

import Shared.Core.Entities.Bullet;
import Shared.Core.Entities.Item;
import Shared.Core.Entities.Soldier;
import Shared.Core.Map.Map;
import Shared.Core.Map.PMSStructs;
import Shared.Core.Math.Calc;
import Shared.Core.Physics.Bullets.BulletTypes;
import Shared.Core.State.StateManager;
import Shared.Core.Types.BulletType;

namespace Soldank
{
namespace
{
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
    const glm::vec2 start_point = bullet.particle.old_position;
    const glm::vec2 end_point = bullet.particle.position;
    float min_distance = std::numeric_limits<float>::max();
    std::optional<BulletCollisionResult> closest_collision;

    for (const int body_part_id : BODY_PARTS_PRIORITY) {
        glm::vec2 body_part_offset =
          soldier.skeleton->GetPos(body_part_id) - soldier.particle.position;
        glm::vec2 collision_position = collision_origin + body_part_offset;

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

        const int sectors_per_axis = map.GetSectorsPerAxis();
        if (sector_x < 0 || sector_x >= sectors_per_axis || sector_y < 0 ||
            sector_y >= sectors_per_axis) {
            continue;
        }

        for (const unsigned int polygon_reference : map.GetSector(sector_x, sector_y).polygons) {
            const unsigned int polygon_id = polygon_reference - 1;
            const PMSPolygon& polygon = map.GetPolygons()[polygon_id];
            if (Map::BulletCollidesWithPolygon(polygon.polygon_type,
                                               static_cast<std::uint8_t>(bullet.team)) &&
                Map::PointInPoly(position, polygon)) {
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

    std::optional<BulletCollisionResult> closest_collision;
    state_manager.ForEachSoldier([&](const Soldier& candidate) {
        if ((bullet.style == BulletType::Fist || bullet.style == BulletType::Blade) &&
            candidate.id == bullet.owner_id) {
            return;
        }

        const auto collision = FindSoldierCollisionPoint(candidate, bullet);
        if (!collision.has_value() ||
            (closest_collision.has_value() && collision->distance >= closest_collision->distance)) {
            return;
        }

        closest_collision = collision;
    });

    if (closest_collision.has_value() && last_hit_distance > -1.0F &&
        closest_collision->distance > last_hit_distance) {
        return std::nullopt;
    }

    return closest_collision;
}

std::optional<BulletCollisionResult> FindColliderCollision(const Bullet& bullet, const Map& map)
{
    constexpr float COLLIDER_RADIUS_SCALE = 1.7F;
    const glm::vec2 start_point = bullet.particle.old_position;
    const glm::vec2 end_point = bullet.particle.position;
    std::optional<BulletCollisionResult> closest_collision;

    for (unsigned int collider_id = 0; collider_id < map.GetColliders().size(); ++collider_id) {
        const PMSCollider& collider = map.GetColliders().at(collider_id);
        if (collider.active == 0) {
            continue;
        }

        const auto hit_position =
          Calc::LineCircleCollision(start_point,
                                    end_point,
                                    { collider.x, collider.y },
                                    collider.radius / COLLIDER_RADIUS_SCALE);
        if (!hit_position.has_value()) {
            continue;
        }

        const float distance = Calc::Vec2Length(*hit_position - start_point);
        if (closest_collision.has_value() && distance >= closest_collision->distance) {
            continue;
        }

        closest_collision = BulletCollisionResult{
            .kind = BulletCollisionKind::MapCollider,
            .position = *hit_position,
            .distance = distance,
            .collider_id = collider_id,
        };
    }

    return closest_collision;
}

std::optional<BulletCollisionResult> FindThingCollision(const Bullet& bullet,
                                                        const StateManager& state_manager)
{
    if (bullet.style == BulletType::FragGrenade) {
        return std::nullopt;
    }

    const glm::vec2 start_point = bullet.particle.old_position;
    const glm::vec2 end_point = bullet.particle.position;
    std::optional<BulletCollisionResult> closest_collision;
    state_manager.ForEachItem([&](const Item& item) {
        if (!item.collide_with_bullets ||
            (item.holding_soldier_id != 0 && item.holding_soldier_id == bullet.owner_id) ||
            item.skeleton == nullptr) {
            return;
        }

        for (unsigned int particle_id = 1; particle_id <= 2; ++particle_id) {
            const auto hit_position = Calc::LineCircleCollision(
              start_point, end_point, item.skeleton->GetPos(particle_id), item.radius);
            if (!hit_position.has_value()) {
                continue;
            }

            const float distance = Calc::Vec2Length(*hit_position - start_point);
            if (closest_collision.has_value() && distance >= closest_collision->distance) {
                continue;
            }

            closest_collision = BulletCollisionResult{
                .kind = BulletCollisionKind::Item,
                .position = *hit_position,
                .distance = distance,
                .item_id = item.id,
            };
        }
    });

    return closest_collision;
}
} // namespace Soldank::BulletCollision
