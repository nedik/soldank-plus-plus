module;

#include <cstdint>
#include <optional>

export module Shared.Core.Physics.Bullets.BulletTypes;

import Extern.Glm;

export namespace Soldank
{
enum class BulletCollisionKind
{
    MapPolygon,
    Soldier,
};

struct BulletCollisionResult
{
    BulletCollisionKind kind;
    glm::vec2 position;
    float distance;
    std::optional<unsigned int> polygon_id;
    std::optional<std::uint8_t> soldier_id;
    std::optional<int> body_part_id;
};
} // namespace Soldank
