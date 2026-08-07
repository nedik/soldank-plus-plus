module;

#include <cstdint>
#include <optional>

export module Shared.Core.Physics.Bullets.BulletTypes;

import Extern.Glm;

export namespace Soldank
{
struct BulletMapCollision
{
    glm::vec2 position;
    unsigned int polygon_id;
};

struct BulletSoldierCollision
{
    std::uint8_t soldier_id;
    int body_part_id;
    glm::vec2 position;
};
} // namespace Soldank
