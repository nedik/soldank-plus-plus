#include "core/map/Map.hpp"

#include "core/map/PMSConstants.hpp"
#include "core/map/PMSEnums.hpp"
#include "core/map/PMSStructs.hpp"

#include "core/math/Calc.hpp"

#include "core/data/IFileReader.hpp"

#include <gtest/gtest.h>

#include <string>
#include <expected>
#include <optional>
#include <array>
#include <span>

constexpr const int VERSION = 1234;
constexpr const std::string_view DESCRIPTION = "test_description";
constexpr const std::string_view TEXTURE_NAME = "test_texture";
const Soldank::PMSColor BACKGROUND_TOP_COLOR{ 1, 2, 3, 4 };
const Soldank::PMSColor BACKGROUND_BOTTOM_COLOR{ 5, 6, 7, 8 };
constexpr const int JET_COUNT = 127;
constexpr const unsigned char GRENADES_COUNT = 3;
constexpr const unsigned char MEDIKITS_COUNT = 6;
constexpr const Soldank::PMSWeatherType WEATHER_TYPE = Soldank::PMSWeatherType::Rain;
constexpr const Soldank::PMSStepType STEP_TYPE = Soldank::PMSStepType::SoftGround;
constexpr const int RANDOM_ID = 12345;
const std::array<Soldank::PMSPolygon, 2> POLYGONS{
    Soldank::PMSPolygon{
      .vertices = { Soldank::PMSVertex{ .x = 1.0F,
                                        .y = 1.0F,
                                        .z = 1.0F,
                                        .rhw = 0.5F,
                                        .color = Soldank::PMSColor(3, 5, 7, 200),
                                        .texture_s = 0.2F,
                                        .texture_t = 0.3F },
                    Soldank::PMSVertex{ .x = 5.0F,
                                        .y = 1.0F,
                                        .z = 1.0F,
                                        .rhw = 0.75F,
                                        .color = Soldank::PMSColor(4, 6, 8, 254),
                                        .texture_s = 0.3F,
                                        .texture_t = 0.6F },
                    Soldank::PMSVertex{ .x = 2.5F,
                                        .y = 5.0F,
                                        .z = 1.0F,
                                        .rhw = 0.25F,
                                        .color = Soldank::PMSColor(0, 0, 0, 0),
                                        .texture_s = 0.9F,
                                        .texture_t = 0.1F } },
      .perpendiculars = { Soldank::PMSVector{ .x = 1.0F, .y = 0.5F, .z = 0.2F },
                          Soldank::PMSVector{ .x = -1.0F, .y = -0.5F, .z = -0.2F },
                          Soldank::PMSVector{ .x = 5.9F, .y = 9.9F, .z = 0.9F } },
      .polygon_type = Soldank::PMSPolygonType::Normal,
      .bounciness = 0.5F },
    Soldank::PMSPolygon{
      .vertices = { Soldank::PMSVertex{}, Soldank::PMSVertex{}, Soldank::PMSVertex{} },
      .perpendiculars = { Soldank::PMSVector{}, Soldank::PMSVector{}, Soldank::PMSVector{} },
      .polygon_type = Soldank::PMSPolygonType::NoCollide,
      .bounciness = 0.8F }
};

class MapReaderExample : public Soldank::IFileReader
{
public:
    std::expected<std::string, Soldank::FileReaderError> Read(
      const std::string& file_path,
      std::ios_base::openmode /*mode*/) const override
    {
        if (file_path != "test_map") {
            return std::unexpected(Soldank::FileReaderError::FileNotFound);
        }

        std::stringstream buffer;
        WriteToBuffer(buffer, VERSION);
        WriteToBuffer(buffer, (unsigned char)DESCRIPTION.length());
        WriteToBuffer(buffer, DESCRIPTION);
        AddFillerToBuffer(buffer, Soldank::DESCRIPTION_MAX_LENGTH - DESCRIPTION.length());
        WriteToBuffer(buffer, (unsigned char)TEXTURE_NAME.length());
        WriteToBuffer(buffer, TEXTURE_NAME);
        AddFillerToBuffer(buffer, Soldank::TEXTURE_NAME_MAX_LENGTH - TEXTURE_NAME.length());

        WriteToBuffer(buffer, BACKGROUND_TOP_COLOR);
        WriteToBuffer(buffer, BACKGROUND_BOTTOM_COLOR);
        WriteToBuffer(buffer, JET_COUNT);
        WriteToBuffer(buffer, GRENADES_COUNT);
        WriteToBuffer(buffer, MEDIKITS_COUNT);
        WriteToBuffer(buffer, WEATHER_TYPE);
        WriteToBuffer(buffer, STEP_TYPE);
        WriteToBuffer(buffer, RANDOM_ID);

        WriteToBuffer(buffer, POLYGONS);

        WriteToBuffer(buffer, (int)0); // sectors_size
        WriteToBuffer(buffer, (int)0); // sectors_count

        WriteToBuffer(buffer, (int)0); // scenery_instances_count
        WriteToBuffer(buffer, (int)0); // scenery_types_count
        WriteToBuffer(buffer, (int)0); // colliders_count
        WriteToBuffer(buffer, (int)0); // spawn_points_count
        WriteToBuffer(buffer, (int)0); // way_points_count

        if (buffer.bad()) {
            return std::unexpected(Soldank::FileReaderError::BufferError);
        }

        return buffer.str();
    }

private:
    template<typename T>
    static const char* Serialize(const T& t)
    {
        // NOLINTNEXTLINE: Serializing data
        return reinterpret_cast<const char*>(&t);
    }

    static void WriteToBuffer(std::stringstream& buffer,
                              const std::array<Soldank::PMSPolygon, 2>& polygons)
    {
        int polygons_count = (int)polygons.size();
        buffer.write(Serialize(polygons_count), sizeof(int));
        for (const Soldank::PMSPolygon& polygon : polygons) {
            for (const Soldank::PMSVertex& vertex : polygon.vertices) {
                buffer.write(Serialize(vertex), sizeof(Soldank::PMSVertex));
            }

            for (const Soldank::PMSVector& perpendicular : polygon.perpendiculars) {
                buffer.write(Serialize(perpendicular), sizeof(Soldank::PMSVector));
            }

            buffer.write(Serialize(polygon.polygon_type), sizeof(Soldank::PMSPolygonType));
        }
    }

    static void WriteToBuffer(std::stringstream& buffer, std::string_view variable_to_write)
    {
        buffer.write(variable_to_write.data(),
                     (std::streamsize)sizeof(char) * (std::streamsize)variable_to_write.size());
    }

    template<typename TVar>
    static void WriteToBuffer(std::stringstream& buffer, TVar variable_to_write)
    {
        buffer.write(Serialize(variable_to_write), sizeof(variable_to_write));
    }

    static void AddFillerToBuffer(std::stringstream& buffer, unsigned int bytes_count)
    {
        char filler = 0;
        for (unsigned int i = 0; i < bytes_count; i++) {
            buffer.write(&filler, sizeof(filler));
        }
    }
};

void ComparePMSColors(const Soldank::PMSColor& color1, const Soldank::PMSColor& color2)
{
    ASSERT_EQ(color1.alpha, color2.alpha);
    ASSERT_EQ(color1.red, color2.red);
    ASSERT_EQ(color1.green, color2.green);
    ASSERT_EQ(color1.blue, color2.blue);
}

void ComparePMSVertices(const Soldank::PMSVertex& vertex1, const Soldank::PMSVertex& vertex2)
{
    ASSERT_EQ(vertex1.x, vertex2.x);
    ASSERT_EQ(vertex1.y, vertex2.y);
    ASSERT_EQ(vertex1.z, vertex2.z);
    ASSERT_EQ(vertex1.rhw, vertex2.rhw);
    ASSERT_EQ(vertex1.texture_s, vertex2.texture_s);
    ASSERT_EQ(vertex1.texture_t, vertex2.texture_t);
    ComparePMSColors(vertex1.color, vertex2.color);
}

void ComparePMSVectors(const Soldank::PMSVector& vector1, const Soldank::PMSVector& vector2)
{
    ASSERT_EQ(vector1.x, vector2.x);
    ASSERT_EQ(vector1.y, vector2.y);
    ASSERT_EQ(vector1.z, vector2.z);
}

void ComparePMSPolygons(const Soldank::PMSPolygon& actual_polygon,
                        const Soldank::PMSPolygon& expected_polygon)
{
    for (unsigned int j = 0; j < 3; j++) {
        ComparePMSVertices(actual_polygon.vertices.at(j), expected_polygon.vertices.at(j));
    }
    for (unsigned int j = 0; j < 3; j++) {
        Soldank::PMSVector expected_perpendiculars = expected_polygon.perpendiculars.at(j);
        expected_perpendiculars.x =
          Soldank::Calc::Vec2Normalize(glm::vec2(expected_polygon.perpendiculars.at(j).x,
                                                 expected_polygon.perpendiculars.at(j).y))
            .x;
        expected_perpendiculars.y =
          Soldank::Calc::Vec2Normalize(glm::vec2(expected_polygon.perpendiculars.at(j).x,
                                                 expected_polygon.perpendiculars.at(j).y))
            .y;
        ComparePMSVectors(actual_polygon.perpendiculars.at(j), expected_perpendiculars);
    }
    ASSERT_EQ(actual_polygon.polygon_type, expected_polygon.polygon_type);
}

void CheckLoadedMap(const Soldank::Map& map)
{
    // TODO: Add missing methods to map and check here
    // ASSERT_EQ(map.GetVersion(), VERSION);
    // ASSERT_EQ(map.GetDescription(), DESCRIPTION);
    ASSERT_EQ(map.GetTextureName(), TEXTURE_NAME);
    ComparePMSColors(map.GetBackgroundTopColor(), BACKGROUND_TOP_COLOR);
    ComparePMSColors(map.GetBackgroundBottomColor(), BACKGROUND_BOTTOM_COLOR);
    ASSERT_EQ(map.GetJetCount(), JET_COUNT);
    // ASSERT_EQ(map.GetGrenadesCount(), GRENADES_COUNT);
    // ASSERT_EQ(map.GetMedikitsCount(), MEDIKITS_COUNT);
    // ASSERT_EQ(map.GetWeatherType(), WEATHER_TYPE);
    // ASSERT_EQ(map.GetStepType(), STEP_TYPE);
    ASSERT_EQ(map.GetPolygons().size(), POLYGONS.size());
    ASSERT_EQ(map.GetPolygonsCount(), POLYGONS.size());
    for (unsigned int i = 0; i < map.GetPolygonsCount(); i++) {
        ComparePMSPolygons(map.GetPolygons().at(i), POLYGONS.at(i));
    }
    // TODO: Add missing map variables
}

TEST(MapTests, TestMapNotFound)
{
    MapReaderExample map_reader;
    Soldank::Map map;
    map.LoadMap("not_existing_map", map_reader);
    ASSERT_EQ(map.GetPolygons().size(), 0); // TODO: should return an error
}

TEST(MapTests, TestMapLoadedCorrectly)
{
    MapReaderExample map_reader;
    Soldank::Map map;
    map.LoadMap("test_map", map_reader);
    CheckLoadedMap(map);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}