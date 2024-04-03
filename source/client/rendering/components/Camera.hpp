#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "core/math/Glm.hpp"

namespace Soldank
{
class Camera
{
public:
    Camera();

    glm::mat4 GetView() const;

    void ZoomIn();
    void ZoomOut();
    void Move(float new_x, float new_y);

    float GetX() const { return camera_position_.x; }

    float GetY() const { return camera_position_.y; }

private:
    static constexpr const unsigned int WIDTH = 640;
    static constexpr const unsigned int HEIGHT = 480;
    static constexpr const float ZOOM_INCREMENT = 20.0;
    static constexpr const float ZOOM_MIN_VALUE = -200.0;
    static constexpr const float ZOOM_MAX_VALUE = 0.0;
    static constexpr const float ZOOM_POWER = 0.01;

    float GetWidth() const;
    float GetHeight() const;

    glm::vec2 camera_position_;
    float zoom_;
};
} // namespace Soldank

#endif
