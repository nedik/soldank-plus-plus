#include "Keyboard.hpp"

#include <GLFW/glfw3.h>

namespace Soldat
{
std::array<bool, 348> Soldat::Keyboard::keys_ = {};
std::array<bool, 348> Soldat::Keyboard::keys_changed_ = {};

void Keyboard::KeyCallback(GLFWwindow* /*window*/,
                           const int key,
                           const int /*scancode*/,
                           const int action,
                           const int /*mods*/)
{
    if (GLFW_RELEASE != action) {
        if (!keys_.at(key)) {
            keys_.at(key) = true;
        }
    } else {
        keys_.at(key) = false;
    }
    keys_changed_.at(key) = action != GLFW_RELEASE;
}

bool Keyboard::Key(const int key)
{
    return keys_.at(key);
}

bool Keyboard::KeyChanged(int key)
{
    const bool ret = keys_changed_.at(key);
    keys_changed_.at(key) = false;
    return ret;
}

bool Keyboard::KeyWentUp(const int key)
{
    return !keys_.at(key) && KeyChanged(key);
}

bool Keyboard::KeyWentDown(const int key)
{
    return keys_.at(key) && KeyChanged(key);
}
} // namespace Soldat