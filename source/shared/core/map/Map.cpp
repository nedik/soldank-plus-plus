#include "Map.hpp"

#include "core/map/Map.hpp"
#include "core/math/Calc.hpp"

#include <array>
#include <spdlog/spdlog.h>
#include <utility>

namespace Soldank
{
void Map::LoadMap(const std::string& map_path)
{
    std::ifstream file(map_path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        spdlog::critical("Map not found {}", map_path);
        return;
    }

    int i = 0;
    int j = 0;
    char filler[64];

    map_data_.boundaries_xy[TopBoundary] = -MAP_BOUNDARY;
    map_data_.boundaries_xy[BottomBoundary] = MAP_BOUNDARY;
    map_data_.boundaries_xy[LeftBoundary] = -MAP_BOUNDARY;
    map_data_.boundaries_xy[RightBoundary] = MAP_BOUNDARY;

    map_data_.polygons_min_x = 0.0F;
    map_data_.polygons_max_x = 0.0F;
    map_data_.polygons_min_y = 0.0F;
    map_data_.polygons_max_y = 0.0F;

    file.read((char*)(&map_data_.version), sizeof(int));

    unsigned char description_length = 0;
    file.read((char*)(&description_length), sizeof(unsigned char));
    char description[DESCRIPTION_MAX_LENGTH];
    memset(description, 0, sizeof(description));
    file.read((char*)(description), sizeof(char) * description_length);
    map_data_.description = description;
    file.read((char*)(filler), sizeof(char) * (DESCRIPTION_MAX_LENGTH - description_length));

    unsigned char texture_name_length = 0;
    file.read((char*)(&texture_name_length), sizeof(unsigned char));
    char texture_name[TEXTURE_NAME_MAX_LENGTH];
    memset(texture_name, 0, sizeof(texture_name));
    file.read((char*)(texture_name), sizeof(char) * texture_name_length);
    map_data_.texture_name = texture_name;
    file.read((char*)(filler), sizeof(char) * (TEXTURE_NAME_MAX_LENGTH - texture_name_length));

    file.read((char*)(&map_data_.background_top_color), sizeof(PMSColor));
    file.read((char*)(&map_data_.background_bottom_color), sizeof(PMSColor));
    file.read((char*)(&map_data_.jet_count), sizeof(int));
    file.read((char*)(&map_data_.grenades_count), sizeof(unsigned char));
    file.read((char*)(&map_data_.medikits_count), sizeof(unsigned char));
    file.read((char*)(&map_data_.weather_type), sizeof(PMSWeatherType));
    file.read((char*)(&map_data_.step_type), sizeof(PMSStepType));
    file.read((char*)(&map_data_.random_id), sizeof(int));

    int polygons_count = 0;
    file.read((char*)(&polygons_count), sizeof(int));
    map_data_.polygons.clear();
    for (i = 0; i < polygons_count; ++i) {
        PMSPolygon tmp;

        for (j = 0; j < 3; ++j) {
            file.read((char*)(&tmp.vertices[j]), sizeof(PMSVertex));

            if (tmp.vertices[j].x < map_data_.polygons_min_x) {
                map_data_.polygons_min_x = tmp.vertices[j].x;
            }
            if (tmp.vertices[j].x > map_data_.polygons_max_x) {
                map_data_.polygons_max_x = tmp.vertices[j].x;
            }

            if (tmp.vertices[j].y < map_data_.polygons_min_y) {
                map_data_.polygons_min_y = tmp.vertices[j].y;
            }
            if (tmp.vertices[j].y > map_data_.polygons_max_y) {
                map_data_.polygons_max_y = tmp.vertices[j].y;
            }
        }
        for (j = 0; j < 3; ++j) {
            file.read((char*)(&tmp.perpendiculars[j]), sizeof(PMSVector));
        }
        tmp.bounciness = glm::length(glm::vec2(tmp.perpendiculars[2].x, tmp.perpendiculars[2].y));

        // TODO: zrobić lepiej
        tmp.perpendiculars[0].x =
          Soldank::Calc::Vec2Normalize(glm::vec2(tmp.perpendiculars[0].x, tmp.perpendiculars[0].y))
            .x;
        tmp.perpendiculars[0].y =
          Soldank::Calc::Vec2Normalize(glm::vec2(tmp.perpendiculars[0].x, tmp.perpendiculars[0].y))
            .y;
        tmp.perpendiculars[1].x =
          Soldank::Calc::Vec2Normalize(glm::vec2(tmp.perpendiculars[1].x, tmp.perpendiculars[1].y))
            .x;
        tmp.perpendiculars[1].y =
          Soldank::Calc::Vec2Normalize(glm::vec2(tmp.perpendiculars[1].x, tmp.perpendiculars[1].y))
            .y;
        tmp.perpendiculars[2].x =
          Soldank::Calc::Vec2Normalize(glm::vec2(tmp.perpendiculars[2].x, tmp.perpendiculars[2].y))
            .x;
        tmp.perpendiculars[2].y =
          Soldank::Calc::Vec2Normalize(glm::vec2(tmp.perpendiculars[2].x, tmp.perpendiculars[2].y))
            .y;

        file.read((char*)(&tmp.polygon_type), sizeof(PMSPolygonType));

        map_data_.polygons.push_back(tmp);
    }

    file.read((char*)(&map_data_.sectors_size), sizeof(int));
    file.read((char*)(&map_data_.sectors_count), sizeof(int));

    auto n = (2 * map_data_.sectors_count + 1) * (2 * map_data_.sectors_count + 1);

    for (i = 0; i < n; i++) {
        unsigned short m;
        file.read((char*)(&m), sizeof(unsigned short));

        PMSSector sector;

        for (int j = 0; j < m; j++) {
            unsigned short poly_id;
            file.read((char*)(&poly_id), sizeof(unsigned short));
            sector.polygons.push_back(poly_id);
        }

        map_data_.sectors2.push_back(sector);
    }

    auto k = 0;
    map_data_.sectors_poly = std::vector<std::vector<PMSSector>>(51, std::vector<PMSSector>(51));

    for (auto& sec_i : map_data_.sectors_poly) {
        for (auto& sec_ij : sec_i) {
            sec_ij = map_data_.sectors2[k];
            k++;
        }
    }

    int scenery_instances_count = 0;
    file.read((char*)(&scenery_instances_count), sizeof(int));
    map_data_.scenery_instances.clear();
    for (i = 0; i < scenery_instances_count; i++) {
        PMSScenery scenery{};

        file.read((char*)(&scenery), sizeof(PMSScenery));
        map_data_.scenery_instances.push_back(scenery);
    }

    int scenery_types_count = 0;
    file.read((char*)(&scenery_types_count), sizeof(int));
    map_data_.scenery_types.clear();
    for (i = 0; i < scenery_types_count; i++) {
        PMSSceneryType scenery_type{};

        char scenery_type_name[SCENERY_NAME_MAX_LENGTH];
        memset(scenery_type_name, 0, sizeof(scenery_type_name));
        file.read((char*)(&scenery_type.name_length), sizeof(unsigned char));
        file.read((char*)(scenery_type_name), sizeof(char) * scenery_type.name_length);
        scenery_type.name = scenery_type_name;
        file.read((char*)(filler),
                  sizeof(char) * (SCENERY_NAME_MAX_LENGTH - scenery_type.name_length));
        file.read((char*)(&scenery_type.timestamp), sizeof(PMSTimestamp));

        map_data_.scenery_types.push_back(scenery_type);
    }

    int colliders_count = 0;
    file.read((char*)(&colliders_count), sizeof(int));
    map_data_.colliders.clear();
    for (i = 0; i < colliders_count; i++) {
        PMSCollider collider{};

        file.read((char*)(&collider), sizeof(PMSCollider));
        map_data_.colliders.push_back(collider);
    }

    int spawn_points_count = 0;
    file.read((char*)(&spawn_points_count), sizeof(int));
    map_data_.spawn_points.clear();
    for (i = 0; i < spawn_points_count; i++) {
        PMSSpawnPoint spawn_point{};

        file.read((char*)(&spawn_point), sizeof(PMSSpawnPoint));
        map_data_.spawn_points.push_back(spawn_point);
    }

    int way_points_count = 0;
    file.read((char*)(&way_points_count), sizeof(way_points_count));
    for (i = 0; i < way_points_count; i++) {
        PMSWayPoint way_point{};

        file.read((char*)(&way_point), sizeof(PMSWayPoint));
        map_data_.way_points.push_back(way_point);
    }

    UpdateBoundaries();

    file.close();
}

void Map::UpdateBoundaries()
{
    map_data_.width = fabs(map_data_.polygons_max_x - map_data_.polygons_min_x);
    map_data_.height = fabs(map_data_.polygons_max_y - map_data_.polygons_min_y);

    map_data_.center_x = floor((map_data_.polygons_min_x + map_data_.polygons_max_x) / 2.0F);
    map_data_.center_y = floor((map_data_.polygons_min_y + map_data_.polygons_max_y) / 2.0F);

    map_data_.boundaries_xy[TopBoundary] = map_data_.polygons_min_y;
    map_data_.boundaries_xy[BottomBoundary] = map_data_.polygons_max_y;
    map_data_.boundaries_xy[LeftBoundary] = map_data_.polygons_min_x;
    map_data_.boundaries_xy[RightBoundary] = map_data_.polygons_max_x;

    if (map_data_.height > map_data_.width) {
        map_data_.boundaries_xy[LeftBoundary] -= (map_data_.height - map_data_.width) / 2.0F;
        map_data_.boundaries_xy[RightBoundary] += (map_data_.height - map_data_.width) / 2.0F;
    } else {
        map_data_.boundaries_xy[TopBoundary] -= (map_data_.width - map_data_.height) / 2.0F;
        map_data_.boundaries_xy[BottomBoundary] += (map_data_.width - map_data_.height) / 2.0F;
    }

    map_data_.boundaries_xy[TopBoundary] -= MAP_BOUNDARY;
    map_data_.boundaries_xy[BottomBoundary] += MAP_BOUNDARY;
    map_data_.boundaries_xy[LeftBoundary] -= MAP_BOUNDARY;
    map_data_.boundaries_xy[RightBoundary] += MAP_BOUNDARY;
}

bool Map::PointInPoly(glm::vec2 p, PMSPolygon poly)
{
    auto a = poly.vertices[0];
    auto b = poly.vertices[1];
    auto c = poly.vertices[2];

    auto ap_x = p.x - a.x;
    auto ap_y = p.y - a.y;
    auto p_ab = (b.x - a.x) * ap_y - (b.y - a.y) * ap_x > 0.0F;
    auto p_ac = (c.x - a.x) * ap_y - (c.y - a.y) * ap_x > 0.0F;

    if (p_ac == p_ab) {
        return false;
    }

    if (((c.x - b.x) * (p.y - b.y) - (c.y - b.y) * (p.x - b.x) > 0.0F) != p_ab) {
        return false;
    }

    return true;
}

bool Map::PointInPolyEdges(float x, float y, int i) const
{
    auto u_x = x - map_data_.polygons[i].vertices[0].x;
    auto u_y = y - map_data_.polygons[i].vertices[0].y;
    auto d = map_data_.polygons[i].perpendiculars[0].x * u_x +
             map_data_.polygons[i].perpendiculars[0].y * u_y;
    if (d < 0.0F) {
        return false;
    }

    u_x = x - map_data_.polygons[i].vertices[1].x;
    u_y = y - map_data_.polygons[i].vertices[1].y;
    d = map_data_.polygons[i].perpendiculars[1].x * u_x +
        map_data_.polygons[i].perpendiculars[1].y * u_y;
    if (d < 0.0F) {
        return false;
    }

    u_x = x - map_data_.polygons[i].vertices[2].x;
    u_y = y - map_data_.polygons[i].vertices[2].y;
    d = map_data_.polygons[i].perpendiculars[2].x * u_x +
        map_data_.polygons[i].perpendiculars[2].y * u_y;
    return d >= 0.0F;
}

glm::vec2 Map::ClosestPerpendicular(int j, glm::vec2 pos, float* d, int* n) const
{
    auto px = std::array{
        map_data_.polygons[j].vertices[0].x,
        map_data_.polygons[j].vertices[1].x,
        map_data_.polygons[j].vertices[2].x,
    };

    auto py = std::array{
        map_data_.polygons[j].vertices[0].y,
        map_data_.polygons[j].vertices[1].y,
        map_data_.polygons[j].vertices[2].y,
    };

    auto p1 = glm::vec2(px[0], py[0]);
    auto p2 = glm::vec2(px[1], py[1]);

    auto d1 = Soldank::Calc::PointLineDistance(p1, p2, pos);

    *d = d1;

    auto edge_v1 = 1;
    auto edge_v2 = 2;

    p1.x = px[1];
    p1.y = py[1];

    p2.x = px[2];
    p2.y = py[2];

    auto d2 = Soldank::Calc::PointLineDistance(p1, p2, pos);

    if (d2 < d1) {
        edge_v1 = 2;
        edge_v2 = 3;
        *d = d2;
    }

    p1.x = px[2];
    p1.y = py[2];

    p2.x = px[0];
    p2.y = py[0];

    auto d3 = Soldank::Calc::PointLineDistance(p1, p2, pos);

    if ((d3 < d2) && (d3 < d1)) {
        edge_v1 = 3;
        edge_v2 = 1;
        *d = d3;
    }

    if (edge_v1 == 1 && edge_v2 == 2) {
        *n = 1;
        return { map_data_.polygons[j].perpendiculars[0].x,
                 map_data_.polygons[j].perpendiculars[0].y };
    }

    if (edge_v1 == 2 && edge_v2 == 3) {
        *n = 2;
        return { map_data_.polygons[j].perpendiculars[1].x,
                 map_data_.polygons[j].perpendiculars[1].y };
    }

    if (edge_v1 == 3 && edge_v2 == 1) {
        *n = 3;
        return { map_data_.polygons[j].perpendiculars[2].x,
                 map_data_.polygons[j].perpendiculars[2].y };
    }

    return { 0.0F, 0.0F };
}
} // namespace Soldank
