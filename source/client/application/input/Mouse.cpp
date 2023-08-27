#include "Mouse.hpp"

#include <GLFW/glfw3.h>

namespace Soldat
{
bool Mouse::first_mouse_ = true;

// initialized in the middle of the screen
double Mouse::x_ = 320.0f;
double Mouse::y_ = 240.0f;

double Mouse::dx_ = 0;
double Mouse::dy_ = 0;

double Mouse::lastx_ = 0;
double Mouse::lasty_ = 0;

double Mouse::scroll_dx_ = 0;
double Mouse::scroll_dy_ = 0;

bool Mouse::buttons_[GLFW_MOUSE_BUTTON_LAST] = { 0 };
bool Mouse::buttons_changed_[GLFW_MOUSE_BUTTON_LAST] = { 0 };

void Mouse::CursorPosCallback(GLFWwindow* window, const double x, const double y)
{
    if (first_mouse_) {
        lastx_ = x;
        lasty_ = y;
        first_mouse_ = false;
    }

    dx_ = x - lastx_;
    dy_ = lasty_ - y; // y coord are inverted
    lastx_ = x;
    lasty_ = y;

    x_ += dx_;
    if (x_ > 640.0) {
        x_ = 640.0;
    }
    if (x_ < 0.0) {
        x_ = 0.0;
    }
    y_ += dy_;
    if (y_ > 480.0) {
        y_ = 480.0;
    }
    if (y_ < 0.0) {
        y_ = 0.0;
    }
}

void Mouse::MouseButtonCallback(GLFWwindow* window, const int button, const int action, int mods)
{
    if (action == GLFW_PRESS) {
        if (!buttons_[button]) {
            buttons_[button] = true;
        }
    } else {
        buttons_[button] = false;
    }
    buttons_changed_[button] = action != GLFW_REPEAT;
}

void Mouse::MouseWheelCallback(GLFWwindow* window, const double dx, const double dy)
{
    Mouse::scroll_dx_ = 0;
    Mouse::scroll_dy_ = 0;
}

double Mouse::GetX()
{
    return x_;
}

double Mouse::GetY()
{
    return y_;
}

double Mouse::GetDx()
{
    const auto temp = dx_;
    dx_ = 0;
    return temp;
}

double Mouse::GetDy()
{
    const auto temp = dy_;
    dy_ = 0;
    return temp;
}

bool Mouse::Button(int button)
{
    return buttons_[button];
}
} // namespace Soldat
