#include "Map.hpp"

#include "core/math/Calc.hpp"

#include <array>

Map::Map(std::string path)
{

    LoadMap(path);
}

void Map::LoadMap(std::string mapPath)
{
    m_isPathSet = true;

    std::ifstream file(mapPath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        // Logger::Append("Map not found - " + mapPath);
        return;
    }

    int i, j;
    char filler[64];

    m_boundariesXY[TOP_BOUNDARY] = -MAP_BOUNDARY;
    m_boundariesXY[BOTTOM_BOUNDARY] = MAP_BOUNDARY;
    m_boundariesXY[LEFT_BOUNDARY] = -MAP_BOUNDARY;
    m_boundariesXY[RIGHT_BOUNDARY] = MAP_BOUNDARY;

    m_polygonsMinX = 0.0f;
    m_polygonsMaxX = 0.0f;
    m_polygonsMinY = 0.0f;
    m_polygonsMaxY = 0.0f;

    file.read((char*)(&m_version), sizeof(int));

    file.read((char*)(&m_descriptionLength), sizeof(unsigned char));
    memset(m_description, 0, sizeof(m_description));
    file.read((char*)(m_description), sizeof(char) * m_descriptionLength);
    file.read((char*)(filler), sizeof(char) * (DESCRIPTION_MAX_LENGTH - m_descriptionLength));

    file.read((char*)(&m_textureNameLength), sizeof(unsigned char));
    memset(m_textureName, 0, sizeof(m_textureName));
    file.read((char*)(m_textureName), sizeof(char) * m_textureNameLength);
    file.read((char*)(filler), sizeof(char) * (TEXTURE_NAME_MAX_LENGTH - m_textureNameLength));

    file.read((char*)(&m_backgroundTopColor), sizeof(PMSColor));
    file.read((char*)(&m_backgroundBottomColor), sizeof(PMSColor));
    file.read((char*)(&m_jetCount), sizeof(int));
    file.read((char*)(&m_grenadesCount), sizeof(unsigned char));
    file.read((char*)(&m_medikitsCount), sizeof(unsigned char));
    file.read((char*)(&m_weatherType), sizeof(PMSWeatherType));
    file.read((char*)(&m_stepType), sizeof(PMSStepType));
    file.read((char*)(&m_randomId), sizeof(int));

    file.read((char*)(&m_polygonsCount), sizeof(int));
    m_polygons.clear();
    for (i = 0; i < m_polygonsCount; ++i) {
        PMSPolygon tmp;

        for (j = 0; j < 3; ++j) {
            file.read((char*)(&tmp.vertices[j]), sizeof(PMSVertex));

            if (tmp.vertices[j].x < m_polygonsMinX) {
                m_polygonsMinX = tmp.vertices[j].x;
            }
            if (tmp.vertices[j].x > m_polygonsMaxX) {
                m_polygonsMaxX = tmp.vertices[j].x;
            }

            if (tmp.vertices[j].y < m_polygonsMinY) {
                m_polygonsMinY = tmp.vertices[j].y;
            }
            if (tmp.vertices[j].y > m_polygonsMaxY) {
                m_polygonsMaxY = tmp.vertices[j].y;
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

        file.read((char*)(&tmp.polygonType), sizeof(PMSPolygonType));

        m_polygons.push_back(tmp);
    }

    file.read((char*)(&m_sectorsSize), sizeof(int));
    file.read((char*)(&m_sectorsCount), sizeof(int));

    auto n = (2 * m_sectorsCount + 1) * (2 * m_sectorsCount + 1);

    for (i = 0; i < n; i++) {
        unsigned short m;
        file.read((char*)(&m), sizeof(unsigned short));

        PMSSector sector;

        for (int j = 0; j < m; j++) {
            unsigned short poly_id;
            file.read((char*)(&poly_id), sizeof(unsigned short));
            sector.polygons.push_back(poly_id);
        }

        sector.polygonsCount = sector.polygons.size();
        sectors.push_back(sector);
    }

    auto k = 0;

    sectors_poly = std::vector<std::vector<PMSSector>>(51, std::vector<PMSSector>(51));

    // for (int sec_i = 0; sec_i < sectors_poly.size(); sec_i++) {
    // 	for (int sec_ij = 0; sec_ij < sectors_poly[sec_i].size(); sec_ij++) {
    // 		sectors_poly[sec_i][sec_ij] = sectors[k];
    // 		k++;
    // 	}
    // }

    for (auto& sec_i : sectors_poly) {
        for (auto& sec_ij : sec_i) {
            sec_ij = sectors[k];
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

    file.read((char*)(&m_sceneryInstancesCount), sizeof(int));
    m_sceneryInstances.clear();
    for (i = 0; i < m_sceneryInstancesCount; ++i) {
        PMSScenery tmp;

        file.read((char*)(&tmp), sizeof(PMSScenery));
        m_sceneryInstances.push_back(tmp);
    }

    file.read((char*)(&m_sceneryTypesCount), sizeof(int));
    m_sceneryTypes.clear();
    for (i = 0; i < m_sceneryTypesCount; ++i) {
        PMSSceneryType tmp;

        memset(tmp.name, 0, sizeof(tmp.name));
        file.read((char*)(&tmp.nameLength), sizeof(unsigned char));
        file.read((char*)(tmp.name), sizeof(char) * tmp.nameLength);
        file.read((char*)(filler), sizeof(char) * (SCENERY_NAME_MAX_LENGTH - tmp.nameLength));
        file.read((char*)(&tmp.timestamp), sizeof(PMSTimestamp));

        m_sceneryTypes.push_back(tmp);
    }

    file.read((char*)(&m_collidersCount), sizeof(int));
    m_colliders.clear();
    for (i = 0; i < m_collidersCount; ++i) {
        PMSCollider tmp;

        file.read((char*)(&tmp), sizeof(PMSCollider));
        m_colliders.push_back(tmp);
    }

    file.read((char*)(&m_spawnPointsCount), sizeof(int));
    m_spawnPoints.clear();
    for (i = 0; i < m_spawnPointsCount; ++i) {
        PMSSpawnPoint tmp;

        file.read((char*)(&tmp), sizeof(PMSSpawnPoint));
        m_spawnPoints.push_back(tmp);
    }

    file.read((char*)(&m_wayPointsCount), sizeof(m_wayPointsCount));
    for (i = 0; i < m_wayPointsCount; ++i) {
        PMSWayPoint tmp;

        file.read((char*)(&tmp), sizeof(PMSWayPoint));
        m_wayPoints.push_back(tmp);
    }

    UpdateBoundaries();

    file.close();
}

void Map::UpdateBoundaries()
{
    m_width = fabs(m_polygonsMaxX - m_polygonsMinX);
    m_height = fabs(m_polygonsMaxY - m_polygonsMinY);

    m_centerX = floor((m_polygonsMinX + m_polygonsMaxX) / 2.0);
    m_centerY = floor((m_polygonsMinY + m_polygonsMaxY) / 2.0);

    m_boundariesXY[TOP_BOUNDARY] = m_polygonsMinY;
    m_boundariesXY[BOTTOM_BOUNDARY] = m_polygonsMaxY;
    m_boundariesXY[LEFT_BOUNDARY] = m_polygonsMinX;
    m_boundariesXY[RIGHT_BOUNDARY] = m_polygonsMaxX;

    if (m_height > m_width) {
        m_boundariesXY[LEFT_BOUNDARY] -= (m_height - m_width) / 2.0;
        m_boundariesXY[RIGHT_BOUNDARY] += (m_height - m_width) / 2.0;
    } else {
        m_boundariesXY[TOP_BOUNDARY] -= (m_width - m_height) / 2.0;
        m_boundariesXY[BOTTOM_BOUNDARY] += (m_width - m_height) / 2.0;
    }

    m_boundariesXY[TOP_BOUNDARY] -= MAP_BOUNDARY;
    m_boundariesXY[BOTTOM_BOUNDARY] += MAP_BOUNDARY;
    m_boundariesXY[LEFT_BOUNDARY] -= MAP_BOUNDARY;
    m_boundariesXY[RIGHT_BOUNDARY] += MAP_BOUNDARY;
}

bool Map::PointInPoly(glm::vec2 p, PMSPolygon poly) const
{
    auto a = poly.vertices[0];
    auto b = poly.vertices[1];
    auto c = poly.vertices[2];

    auto ap_x = p.x - a.x;
    auto ap_y = p.y - a.y;
    auto p_ab = (b.x - a.x) * ap_y - (b.y - a.y) * ap_x > 0.0f;
    auto p_ac = (c.x - a.x) * ap_y - (c.y - a.y) * ap_x > 0.0f;

    if (p_ac == p_ab) {
        return false;
    }

    if (((c.x - b.x) * (p.y - b.y) - (c.y - b.y) * (p.x - b.x) > 0.0f) != p_ab) {
        return false;
    }

    return true;
}

bool Map::PointInPolyEdges(float x, float y, int i) const
{
    auto u_x = x - m_polygons[i].vertices[0].x;
    auto u_y = y - m_polygons[i].vertices[0].y;
    auto d = m_polygons[i].perpendiculars[0].x * u_x + m_polygons[i].perpendiculars[0].y * u_y;
    if (d < 0.0f) {
        return false;
    }

    u_x = x - m_polygons[i].vertices[1].x;
    u_y = y - m_polygons[i].vertices[1].y;
    d = m_polygons[i].perpendiculars[1].x * u_x + m_polygons[i].perpendiculars[1].y * u_y;
    if (d < 0.0f) {
        return false;
    }

    u_x = x - m_polygons[i].vertices[2].x;
    u_y = y - m_polygons[i].vertices[2].y;
    d = m_polygons[i].perpendiculars[2].x * u_x + m_polygons[i].perpendiculars[2].y * u_y;
    if (d < 0.0f) {
        return false;
    }

    return true;
}

glm::vec2 Map::ClosestPerpendicular(int j, glm::vec2 pos, float* d, int* n) const
{
    auto px = std::array{
        m_polygons[j].vertices[0].x,
        m_polygons[j].vertices[1].x,
        m_polygons[j].vertices[2].x,
    };

    auto py = std::array{
        m_polygons[j].vertices[0].y,
        m_polygons[j].vertices[1].y,
        m_polygons[j].vertices[2].y,
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
        return glm::vec2(m_polygons[j].perpendiculars[0].x, m_polygons[j].perpendiculars[0].y);
    }

    if (edge_v1 == 2 && edge_v2 == 3) {
        *n = 2;
        return glm::vec2(m_polygons[j].perpendiculars[1].x, m_polygons[j].perpendiculars[1].y);
    }

    if (edge_v1 == 3 && edge_v2 == 1) {
        *n = 3;
        return glm::vec2(m_polygons[j].perpendiculars[2].x, m_polygons[j].perpendiculars[2].y);
    }

    return glm::vec2(0.0f, 0.0f);
}
