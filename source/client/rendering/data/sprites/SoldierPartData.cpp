#include "SoldierPartData.hpp"

namespace Soldat::Sprites
{
SoldierPartData::SoldierPartData(const Texture::TextureData& texture_data,
                                 glm::uvec2 point,
                                 glm::vec2 center,
                                 bool visible,
                                 const std::optional<Texture::TextureData>& flipped_texture_data,
                                 bool team,
                                 float flexibility,
                                 SoldierColor color,
                                 SoldierAlpha alpha)
    : point_(point)
    , center_(center)
    , visible_(visible)
    , flip_(flipped_texture_data.has_value())
    , team_(team)
    , flexibility_(flexibility)
    , color_(color)
    , alpha_(alpha)
    , texture_(texture_data.opengl_id)
    , texture_width_(texture_data.width)
    , texture_height_(texture_data.height)
{
    if (flipped_texture_data.has_value()) {
        texture_flipped_ = flipped_texture_data->opengl_id;
        texture_flipped_width_ = flipped_texture_data->width;
        texture_flipped_height_ = flipped_texture_data->height;
    }
}
} // namespace Soldat::Sprites
