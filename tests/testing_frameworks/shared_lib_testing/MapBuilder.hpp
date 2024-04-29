#ifndef __MAP_BUILDER_HPP__
#define __MAP_BUILDER_HPP__

#include "core/map/Map.hpp"
#include "core/map/PMSEnums.hpp"

#include <memory>

namespace SoldankTesting
{
class MapBuilder
{
public:
    static std::unique_ptr<MapBuilder> Empty();
    MapBuilder* AddPolygon(glm::vec2 vertex1,
                           glm::vec2 vertex2,
                           glm::vec2 vertex3,
                           Soldank::PMSPolygonType polygon_type);
    std::unique_ptr<Soldank::Map> Build();

private:
    MapBuilder() = default;

    void UpdateBoundaries();
    void GenerateSectors();
    bool IsPolygonInSector(unsigned short polygon_index,
                           float sector_x,
                           float sector_y,
                           float sector_size);
    static bool SegmentsIntersect(glm::vec2 a1, glm::vec2 b1, glm::vec2 a2, glm::vec2 b2);

    Soldank::MapData map_data_;
    float polygons_min_x_{};
    float polygons_max_x_{};
    float polygons_min_y_{};
    float polygons_max_y_{};
    float width_{};
    float height_{};
};
} // namespace SoldankTesting

#endif
