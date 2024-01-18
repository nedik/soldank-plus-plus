#ifndef __MAP_HPP__
#define __MAP_HPP__

#include "PMSConstants.hpp"
#include "PMSEnums.hpp"
#include "PMSStructs.hpp"

#include "core/math/Glm.hpp"

#include <vector>
#include <fstream>
#include <cstring>
#include <span>
#include <array>

namespace Soldat
{
class Map
{
public:
    Map(const std::string& path);

    void LoadMap(const std::string& map_path);

    static bool PointInPoly(glm::vec2 p, PMSPolygon poly);
    bool PointInPolyEdges(float x, float y, int i) const;
    glm::vec2 ClosestPerpendicular(int j, glm::vec2 pos, float* d, int* n) const;

    PMSColor GetBackgroundTopColor() { return background_top_color_; }

    PMSColor GetBackgroundBottomColor() { return background_bottom_color_; }

    std::span<float, 4> GetBoundaries() { return boundaries_xy_; }

    const std::vector<PMSPolygon>& GetPolygons() const { return polygons_; }

    unsigned int GetPolygonsCount() { return polygons_.size(); }

    const std::vector<PMSScenery>& GetSceneryInstances() const { return scenery_instances_; }

    const std::vector<PMSSceneryType>& GetSceneryTypes() const { return scenery_types_; }

    const std::vector<PMSSpawnPoint>& GetSpawnPoints() const { return spawn_points_; }

    const std::string& GetTextureName() const { return texture_name_; }

    int GetJetCount() const { return jet_count_; }

    int GetSectorsSize() const { return sectors_size_; }

    int GetSectorsCount() const { return sectors_count_; }

    const PMSSector& GetSector(unsigned int x, unsigned int y) const { return sectors_poly_[x][y]; }

    enum MapBoundary
    {
        TopBoundary = 0,
        BottomBoundary,
        LeftBoundary,
        RightBoundary
    };

private:
    std::array<float, 4> boundaries_xy_;
    float polygons_min_x_;
    float polygons_max_x_;
    float polygons_min_y_;
    float polygons_max_y_;
    float width_;
    float height_;
    float center_x_;
    float center_y_;

    int version_;

    std::string description_;
    std::string texture_name_;

    PMSColor background_top_color_;
    PMSColor background_bottom_color_;

    int jet_count_;
    unsigned char grenades_count_;
    unsigned char medikits_count_;
    PMSWeatherType weather_type_;
    PMSStepType step_type_;
    int random_id_;

    std::vector<PMSPolygon> polygons_;

    int sectors_size_;
    int sectors_count_;

    std::vector<PMSSector> sectors2_;
    std::vector<std::vector<PMSSector>> sectors_poly_;
    std::vector<PMSScenery> scenery_instances_;
    std::vector<PMSSceneryType> scenery_types_;
    std::vector<PMSCollider> colliders_;
    std::vector<PMSSpawnPoint> spawn_points_;
    std::vector<PMSWayPoint> way_points_;

    void UpdateBoundaries();
};
} // namespace Soldat

#endif
