#include "SoldierPartData.hpp"

#include "rendering/data/Texture.hpp"

namespace Soldat::Sprites
{
SoldierPartData::SoldierPartData(const std::filesystem::path& file_path,
                                 glm::uvec2 point,
                                 glm::vec2 center,
                                 bool visible,
                                 std::optional<std::string> flipped_file_path,
                                 bool team,
                                 float flexibility,
                                 SoldierColor color,
                                 SoldierAlpha alpha)
    : file_path_(file_path)
    , point_(point)
    , center_(center)
    , visible_(visible)
    , flip_(flipped_file_path.has_value())
    , team_(team)
    , flexibility_(flexibility)
    , color_(color)
    , alpha_(alpha)
    , texture_width_(0)
    , texture_height_(0)
    , texture_flipped_width_(0)
    , texture_flipped_height_(0)
{
    auto texture_data = Texture::Load(file_path.string().c_str());
    texture_ = texture_data.opengl_id;
    texture_width_ = texture_data.width;
    texture_height_ = texture_data.height;
    if (flipped_file_path.has_value()) {
        auto texture_flipped_data = Texture::Load(flipped_file_path->c_str());
        texture_flipped_ = texture_flipped_data.opengl_id;
        texture_flipped_width_ = texture_flipped_data.width;
        texture_flipped_height_ = texture_flipped_data.height;
    }
}

SoldierPartData::~SoldierPartData()
{
    Texture::Delete(texture_);
    if (flip_) {
        Texture::Delete(texture_flipped_);
    }
}
} // namespace Soldat::Sprites
