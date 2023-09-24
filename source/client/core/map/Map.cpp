#include "Map.hpp"

#include "core/map/Map.hpp"
#include "core/math/Calc.hpp"

#include <array>
#include <utility>

namespace Soldat
{
Map::Map(const std::string& path)
{
    LoadMap(path);
}

void Map::LoadMap(const std::string& map_path)
{
    std::ifstream file(map_path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        // Logger::Append("Map not found - " + mapPath);
        return;
    }

    int i = 0;
    int j = 0;
    char filler[64];

    boundaries_xy_[TopBoundary] = -MAP_BOUNDARY;
    boundaries_xy_[BottomBoundary] = MAP_BOUNDARY;
    boundaries_xy_[LeftBoundary] = -MAP_BOUNDARY;
    boundaries_xy_[RightBoundary] = MAP_BOUNDARY;

    polygons_min_x_ = 0.0F;
    polygons_max_x_ = 0.0F;
    polygons_min_y_ = 0.0F;
    polygons_max_y_ = 0.0F;

    file.read((char*)(&version_), sizeof(int));

    unsigned char description_length = 0;
    file.read((char*)(&description_length), sizeof(unsigned char));
    char description[DESCRIPTION_MAX_LENGTH];
    memset(description, 0, sizeof(description));
    file.read((char*)(description), sizeof(char) * description_length);
    description_ = description;
    file.read((char*)(filler), sizeof(char) * (DESCRIPTION_MAX_LENGTH - description_length));

    unsigned char texture_name_length = 0;
    file.read((char*)(&texture_name_length), sizeof(unsigned char));
    char texture_name[TEXTURE_NAME_MAX_LENGTH];
    memset(texture_name, 0, sizeof(texture_name));
    file.read((char*)(texture_name), sizeof(char) * texture_name_length);
    texture_name_ = texture_name;
    file.read((char*)(filler), sizeof(char) * (TEXTURE_NAME_MAX_LENGTH - texture_name_length));

    file.read((char*)(&background_top_color_), sizeof(PMSColor));
    file.read((char*)(&background_bottom_color_), sizeof(PMSColor));
    file.read((char*)(&jet_count_), sizeof(int));
    file.read((char*)(&grenades_count_), sizeof(unsigned char));
    file.read((char*)(&medikits_count_), sizeof(unsigned char));
    file.read((char*)(&weather_type_), sizeof(PMSWeatherType));
    file.read((char*)(&step_type_), sizeof(PMSStepType));
    file.read((char*)(&random_id_), sizeof(int));

    int polygons_count = 0;
    file.read((char*)(&polygons_count), sizeof(int));
    polygons_.clear();
    for (i = 0; i < polygons_count; ++i) {
        PMSPolygon tmp;

        for (j = 0; j < 3; ++j) {
            file.read((char*)(&tmp.vertices[j]), sizeof(PMSVertex));

            if (tmp.vertices[j].x < polygons_min_x_) {
                polygons_min_x_ = tmp.vertices[j].x;
            }
            if (tmp.vertices[j].x > polygons_max_x_) {
                polygons_max_x_ = tmp.vertices[j].x;
            }

            if (tmp.vertices[j].y < polygons_min_y_) {
                polygons_min_y_ = tmp.vertices[j].y;
            }
            if (tmp.vertices[j].y > polygons_max_y_) {
                polygons_max_y_ = tmp.vertices[j].y;
            }
        }
        for (j = 0; j < 3; ++j) {
            file.read((char*)(&tmp.perpendiculars[j]), sizeof(PMSVector));
        }
        tmp.bounciness = glm::length(glm::vec2(tmp.perpendiculars[2].x, tmp.perpendiculars[2].y));

        // TODO: zrobić lepiej
        tmp.perpendiculars[0].x =
          Soldat::Calc::Vec2Normalize(glm::vec2(tmp.perpendiculars[0].x, tmp.perpendiculars[0].y))
            .x;
        tmp.perpendiculars[0].y =
          Soldat::Calc::Vec2Normalize(glm::vec2(tmp.perpendiculars[0].x, tmp.perpendiculars[0].y))
            .y;
        tmp.perpendiculars[1].x =
          Soldat::Calc::Vec2Normalize(glm::vec2(tmp.perpendiculars[1].x, tmp.perpendiculars[1].y))
            .x;
        tmp.perpendiculars[1].y =
          Soldat::Calc::Vec2Normalize(glm::vec2(tmp.perpendiculars[1].x, tmp.perpendiculars[1].y))
            .y;
        tmp.perpendiculars[2].x =
          Soldat::Calc::Vec2Normalize(glm::vec2(tmp.perpendiculars[2].x, tmp.perpendiculars[2].y))
            .x;
        tmp.perpendiculars[2].y =
          Soldat::Calc::Vec2Normalize(glm::vec2(tmp.perpendiculars[2].x, tmp.perpendiculars[2].y))
            .y;

        file.read((char*)(&tmp.polygon_type), sizeof(PMSPolygonType));

        polygons_.push_back(tmp);
    }

    file.read((char*)(&sectors_size_), sizeof(int));
    file.read((char*)(&sectors_count_), sizeof(int));

    auto n = (2 * sectors_count_ + 1) * (2 * sectors_count_ + 1);

    for (i = 0; i < n; i++) {
        unsigned short m;
        file.read((char*)(&m), sizeof(unsigned short));

        PMSSector sector;

        for (int j = 0; j < m; j++) {
            unsigned short poly_id;
            file.read((char*)(&poly_id), sizeof(unsigned short));
            sector.polygons.push_back(poly_id);
        }

        sector.polygons_count = sector.polygons.size();
        sectors2_.push_back(sector);
    }

    auto k = 0;

    sectors_poly_ = std::vector<std::vector<PMSSector>>(51, std::vector<PMSSector>(51));

    // for (int sec_i = 0; sec_i < sectors_poly.size(); sec_i++) {
    // 	for (int sec_ij = 0; sec_ij < sectors_poly[sec_i].size(); sec_ij++) {
    // 		sectors_poly[sec_i][sec_ij] = sectors[k];
    // 		k++;
    // 	}
    // }

    for (auto& sec_i : sectors_poly_) {
        for (auto& sec_ij : sec_i) {
            sec_ij = sectors2_[k];
            k++;
            if (k >= 51 * 51) {
                break;
            }
        }
        if (k >= 51 * 51) {
            break;
        }
    }

    // TODO: w soldanku jest inaczej
    // for (int x = 0; x <= m_sectorsCount * 2; ++x)
    // {
    // 	for (int y = 0; y <= m_sectorsCount * 2; ++y)
    // 	{
    // 		file.read((char *)(&m_sectors[x][y].polygonsCount), sizeof(unsigned short));

    // 		for (i = 0; i < m_sectors[x][y].polygonsCount; ++i)
    // 		{
    // 			unsigned short tmpPolygonIndex;
    // 			file.read((char *)(&tmpPolygonIndex), sizeof(unsigned short));
    // 			m_sectors[x][y].polygons.push_back(tmpPolygonIndex);
    // 		}
    // 	}
    // }

    int scenery_instances_count = 0;
    file.read((char*)(&scenery_instances_count), sizeof(int));
    scenery_instances_.clear();
    for (i = 0; i < scenery_instances_count; i++) {
        PMSScenery scenery{};

        file.read((char*)(&scenery), sizeof(PMSScenery));
        scenery_instances_.push_back(scenery);
    }

    int scenery_types_count = 0;
    file.read((char*)(&scenery_types_count), sizeof(int));
    scenery_types_.clear();
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

        scenery_types_.push_back(scenery_type);
    }

    int colliders_count = 0;
    file.read((char*)(&colliders_count), sizeof(int));
    colliders_.clear();
    for (i = 0; i < colliders_count; i++) {
        PMSCollider collider{};

        file.read((char*)(&collider), sizeof(PMSCollider));
        colliders_.push_back(collider);
    }

    int spawn_points_count = 0;
    file.read((char*)(&spawn_points_count), sizeof(int));
    spawn_points_.clear();
    for (i = 0; i < spawn_points_count; i++) {
        PMSSpawnPoint spawn_point{};

        file.read((char*)(&spawn_point), sizeof(PMSSpawnPoint));
        spawn_points_.push_back(spawn_point);
    }

    int way_points_count = 0;
    file.read((char*)(&way_points_count), sizeof(way_points_count));
    for (i = 0; i < way_points_count; i++) {
        PMSWayPoint way_point{};

        file.read((char*)(&way_point), sizeof(PMSWayPoint));
        way_points_.push_back(way_point);
    }

    UpdateBoundaries();

    file.close();
}

void Map::UpdateBoundaries()
{
    width_ = fabs(polygons_max_x_ - polygons_min_x_);
    height_ = fabs(polygons_max_y_ - polygons_min_y_);

    center_x_ = floor((polygons_min_x_ + polygons_max_x_) / 2.0F);
    center_y_ = floor((polygons_min_y_ + polygons_max_y_) / 2.0F);

    boundaries_xy_[TopBoundary] = polygons_min_y_;
    boundaries_xy_[BottomBoundary] = polygons_max_y_;
    boundaries_xy_[LeftBoundary] = polygons_min_x_;
    boundaries_xy_[RightBoundary] = polygons_max_x_;

    if (height_ > width_) {
        boundaries_xy_[LeftBoundary] -= (height_ - width_) / 2.0F;
        boundaries_xy_[RightBoundary] += (height_ - width_) / 2.0F;
    } else {
        boundaries_xy_[TopBoundary] -= (width_ - height_) / 2.0F;
        boundaries_xy_[BottomBoundary] += (width_ - height_) / 2.0F;
    }

    boundaries_xy_[TopBoundary] -= MAP_BOUNDARY;
    boundaries_xy_[BottomBoundary] += MAP_BOUNDARY;
    boundaries_xy_[LeftBoundary] -= MAP_BOUNDARY;
    boundaries_xy_[RightBoundary] += MAP_BOUNDARY;
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
    auto u_x = x - polygons_[i].vertices[0].x;
    auto u_y = y - polygons_[i].vertices[0].y;
    auto d = polygons_[i].perpendiculars[0].x * u_x + polygons_[i].perpendiculars[0].y * u_y;
    if (d < 0.0F) {
        return false;
    }

    u_x = x - polygons_[i].vertices[1].x;
    u_y = y - polygons_[i].vertices[1].y;
    d = polygons_[i].perpendiculars[1].x * u_x + polygons_[i].perpendiculars[1].y * u_y;
    if (d < 0.0F) {
        return false;
    }

    u_x = x - polygons_[i].vertices[2].x;
    u_y = y - polygons_[i].vertices[2].y;
    d = polygons_[i].perpendiculars[2].x * u_x + polygons_[i].perpendiculars[2].y * u_y;
    return d >= 0.0F;
}

glm::vec2 Map::ClosestPerpendicular(int j, glm::vec2 pos, float* d, int* n) const
{
    auto px = std::array{
        polygons_[j].vertices[0].x,
        polygons_[j].vertices[1].x,
        polygons_[j].vertices[2].x,
    };

    auto py = std::array{
        polygons_[j].vertices[0].y,
        polygons_[j].vertices[1].y,
        polygons_[j].vertices[2].y,
    };

    auto p1 = glm::vec2(px[0], py[0]);
    auto p2 = glm::vec2(px[1], py[1]);

    auto d1 = Soldat::Calc::PointLineDistance(p1, p2, pos);

    *d = d1;

    auto edge_v1 = 1;
    auto edge_v2 = 2;

    p1.x = px[1];
    p1.y = py[1];

    p2.x = px[2];
    p2.y = py[2];

    auto d2 = Soldat::Calc::PointLineDistance(p1, p2, pos);

    if (d2 < d1) {
        edge_v1 = 2;
        edge_v2 = 3;
        *d = d2;
    }

    p1.x = px[2];
    p1.y = py[2];

    p2.x = px[0];
    p2.y = py[0];

    auto d3 = Soldat::Calc::PointLineDistance(p1, p2, pos);

    if ((d3 < d2) && (d3 < d1)) {
        edge_v1 = 3;
        edge_v2 = 1;
        *d = d3;
    }

    if (edge_v1 == 1 && edge_v2 == 2) {
        *n = 1;
        return { polygons_[j].perpendiculars[0].x, polygons_[j].perpendiculars[0].y };
    }

    if (edge_v1 == 2 && edge_v2 == 3) {
        *n = 2;
        return { polygons_[j].perpendiculars[1].x, polygons_[j].perpendiculars[1].y };
    }

    if (edge_v1 == 3 && edge_v2 == 1) {
        *n = 3;
        return { polygons_[j].perpendiculars[2].x, polygons_[j].perpendiculars[2].y };
    }

    return { 0.0F, 0.0F };
}
} // namespace Soldat
