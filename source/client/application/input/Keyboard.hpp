#ifndef __KEYBOARD_HPP__
#define __KEYBOARD_HPP__

#include <array>

struct GLFWwindow;

namespace Soldat
{
class Keyboard
{
public:
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static bool Key(int key);
    static bool KeyChanged(int key);
    static bool KeyWentUp(int key);
    static bool KeyWentDown(int key);

private:
    static std::array<bool, 348> keys_;
    static std::array<bool, 348> keys_changed_;
};
} // namespace Soldat

#endif
