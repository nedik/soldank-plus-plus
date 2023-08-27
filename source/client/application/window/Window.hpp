#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include <string>
#include <cstdint>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Soldat
{
enum class CursorMode : uint8_t { Locked, Normal, Hidden };

class Window
{
public:
    Window();
    Window(std::string title, int width, int height);
    Window(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&) = delete;

    ~Window();

    void SetCursorMode(CursorMode cursor_mode) const;
    void SetTitle(const char* title) const;
    void SetWindowSize(uint32_t width, uint32_t height) const;
    float GetAspectRatio() const;

    void Create();
    void Close();
    void Destroy();

    void MakeContextCurrent();
    static void PollInput();
    void SwapBuffers();
    bool ShouldClose();

    void ResizeCallback(GLFWwindow* window, int width, int height);
    static void GLFWErrorCallback(int error, const char* description);

private:
    int width_;
    int height_;
    std::string title_;
    GLFWwindow* glfw_window_;
};
} // namespace Soldat

#endif
