#pragma once

#include "PMSConstants.hpp"
#include "PMSEnums.hpp"
#include "PMSStructs.hpp"

#include "core/math/Glm.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <string.h>
#include <span>

class Map
{
public:
    Map(){};

    Map(std::string path);

    void LoadMap(std::string mapPath);

    bool PointInPoly(glm::vec2 p, PMSPolygon poly) const;
    bool PointInPolyEdges(float x, float y, int i) const;
    glm::vec2 ClosestPerpendicular(int j, glm::vec2 pos, float* d, int* n) const;

    PMSColor GetBackgroundTopColor() { return m_backgroundTopColor; }

    PMSColor GetBackgroundBottomColor() { return m_backgroundBottomColor; }

    std::span<float, 4> GetBoundaries() { return m_boundariesXY; }

    const std::vector<PMSPolygon>& GetPolygons() const { return m_polygons; }

    unsigned int GetPolygonsCount() { return m_polygons.size(); }

    const std::vector<PMSScenery>& GetSceneryInstances() const { return m_sceneryInstances; }

    const std::vector<PMSSceneryType>& GetSceneryTypes() const { return m_sceneryTypes; }

    const std::vector<PMSSpawnPoint>& GetSpawnPoints() const { return m_spawnPoints; }

    const char* GetTextureName() const { return m_textureName; }

    int GetJetCount() const { return m_jetCount; }

    int GetSectorsSize() const { return m_sectorsSize; }

    int GetSectorsCount() const { return m_sectorsCount; }

    const PMSSector& GetSector(unsigned int x, unsigned int y) const { return sectors_poly[x][y]; }

    enum MapBoundary { TOP_BOUNDARY = 0, BOTTOM_BOUNDARY, LEFT_BOUNDARY, RIGHT_BOUNDARY };

private:
    float m_boundariesXY[4];
    bool m_isPathSet;
    float m_polygonsMinX, m_polygonsMaxX, m_polygonsMinY, m_polygonsMaxY, m_width, m_height,
      m_centerX, m_centerY;

    int m_version;

    unsigned char m_descriptionLength;
    char m_description[DESCRIPTION_MAX_LENGTH];

    unsigned char m_textureNameLength;
    char m_textureName[TEXTURE_NAME_MAX_LENGTH];

    PMSColor m_backgroundTopColor, m_backgroundBottomColor;

    int m_jetCount;
    unsigned char m_grenadesCount, m_medikitsCount;
    PMSWeatherType m_weatherType;
    PMSStepType m_stepType;
    int m_randomId;

    int m_polygonsCount;
    std::vector<PMSPolygon> m_polygons;

    int m_sectorsSize;
    int m_sectorsCount;
    PMSSector m_sectors[SECTORS_COUNT][SECTORS_COUNT];

    std::vector<PMSSector> sectors;
    std::vector<std::vector<PMSSector>> sectors_poly;

    int m_sceneryInstancesCount;
    std::vector<PMSScenery> m_sceneryInstances;

    int m_sceneryTypesCount;
    std::vector<PMSSceneryType> m_sceneryTypes;

    int m_collidersCount;
    std::vector<PMSCollider> m_colliders;

    int m_spawnPointsCount;
    std::vector<PMSSpawnPoint> m_spawnPoints;

    int m_wayPointsCount;
    std::vector<PMSWayPoint> m_wayPoints;

    void UpdateBoundaries();
};
