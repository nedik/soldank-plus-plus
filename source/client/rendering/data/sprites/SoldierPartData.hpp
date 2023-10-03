#ifndef __SOLDIER_PART_DATA_HPP__
#define __SOLDIER_PART_DATA_HPP__

#include "rendering/data/sprites/SpriteTypes.hpp"

#include "core/math/Glm.hpp"

#include <string>
#include <vector>
#include <filesystem>
#include <optional>
#include <variant>

namespace Soldat::Sprites
{

class SoldierPartData
{
public:
    SoldierPartData(const std::filesystem::path& file_path,
                    glm::uvec2 point,
                    glm::vec2 center,
                    bool visible,
                    std::optional<std::string> flipped_file_path,
                    bool team,
                    float flexibility,
                    SoldierColor color,
                    SoldierAlpha alpha);
    ~SoldierPartData();

    // it's not safe to be able to copy/move this because we would also need to take care of the
    // created OpenGL buffers and textures
    SoldierPartData(const SoldierPartData&) = delete;
    SoldierPartData& operator=(SoldierPartData other) = delete;
    SoldierPartData(SoldierPartData&&) = delete;
    SoldierPartData& operator=(SoldierPartData&& other) = delete;

    glm::uvec2 GetPoint() const { return point_; }
    glm::vec2 GetCenter() const { return center_; }

    bool IsVisible() const { return visible_; }
    bool IsFlippable() const { return flip_; }

    float GetFlexibility() const { return flexibility_; }

    SoldierColor GetSoldierColor() const { return color_; }

    SoldierAlpha GetSoldierAlpha() const { return alpha_; }

    unsigned int GetTexture() const { return texture_; }
    unsigned int GetTextureFlipped() const { return texture_flipped_; }

    int GetTextureWidth() const { return texture_width_; }
    int GetTextureHeight() const { return texture_height_; }

    int GetTextureFlippedWidth() const { return texture_flipped_width_; }
    int GetTextureFlippedHeight() const { return texture_flipped_height_; }

private:
    std::filesystem::path file_path_;
    glm::uvec2 point_;
    glm::vec2 center_;
    bool visible_;
    bool flip_;
    bool team_;
    float flexibility_;
    SoldierColor color_;
    SoldierAlpha alpha_;
    unsigned int texture_;
    unsigned int texture_flipped_;
    int texture_width_;
    int texture_height_;
    int texture_flipped_width_;
    int texture_flipped_height_;
};
} // namespace Soldat::Sprites

#endif
