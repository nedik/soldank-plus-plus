#include "Window.hpp"

#include "application/config/Config.hpp"
#include "application/input/Keyboard.hpp"
#include "application/input/Mouse.hpp"
#include "application/window/Window.hpp"

#include <GLFW/glfw3.h>
#include <iostream>

namespace Soldat
{
Window::Window()
    : Window("Soldat++", Config::INITIAL_WINDOW_WIDTH, Config::INITIAL_WINDOWS_HEIGHT)
{
}

Window::Window(std::string title, int width, int height)
    : title_(std::move(title))
    , glfw_window_(nullptr)
{
    if (glfwInit() == 0) {
        std::cerr << "Error: Failed to initialize GLFW" << std::endl;
        throw "Error: Failed to initialize GLFW";
    }

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    width_ = width;
    height_ = height;
}

Window::~Window()
{
    glfwTerminate();
}

void Window::Create()
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (monitor == nullptr) {
        std::cerr << "Error: Failed to get primary monitor\n";
        throw "Error: Failed to get primary monitor";
    }

    glfw_window_ =
      glfwCreateWindow(width_, height_, title_.c_str(), /* monitor */ nullptr, nullptr);
    if (nullptr == glfw_window_) {
        std::cerr << "Error: Failed to create window.\n";
        throw "Error: Failed to create window.";
    }

    glfwMakeContextCurrent(glfw_window_);

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0) {
        std::cerr << "Error: Failed to initialize GLAD." << std::endl;
        glfwTerminate();
        throw "Error: Failed to initialize GLAD.";
    }

    glfwSetWindowUserPointer(glfw_window_, this);
    glfwSetFramebufferSizeCallback(
      glfw_window_, [](GLFWwindow* glfw_window, int width, int height) {
          auto* window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
          window->ResizeCallback(glfw_window, width, height);
      });
    glfwSetKeyCallback(glfw_window_, Keyboard::KeyCallback);
    glfwSetCursorPosCallback(glfw_window_, Mouse::CursorPosCallback);
    glfwSetScrollCallback(glfw_window_, Mouse::MouseWheelCallback);
    glfwSetMouseButtonCallback(glfw_window_, Mouse::MouseButtonCallback);
    glfwSetErrorCallback(GLFWErrorCallback);

    SetCursorMode(CursorMode::Locked);
}

void Window::SetCursorMode(CursorMode cursor_mode) const
{
    int glfw_cursor_mode = GLFW_CURSOR_HIDDEN;
    switch (cursor_mode) {
        case CursorMode::Locked:
            glfw_cursor_mode = GLFW_CURSOR_DISABLED;
            break;
        case CursorMode::Normal:
            glfw_cursor_mode = GLFW_CURSOR_NORMAL;
            break;
        case CursorMode::Hidden:
            glfw_cursor_mode = GLFW_CURSOR_HIDDEN;
            break;
    }
    glfwSetInputMode(glfw_window_, GLFW_CURSOR, glfw_cursor_mode);
}

void Window::SetTitle(const char* title) const
{
    glfwSetWindowTitle(glfw_window_, title);
}

void Window::SetWindowSize(const uint32_t /*width*/, const uint32_t /*height*/) const
{
    glfwSetWindowSize(glfw_window_, width_, height_);
}

float Window::GetAspectRatio() const
{
    return static_cast<float>(width_) / static_cast<float>(height_);
}

void Window::MakeContextCurrent()
{
    glfwMakeContextCurrent(glfw_window_);
}

void Window::PollInput()
{
    glfwPollEvents();
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(glfw_window_);
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(glfw_window_) != 0;
}

void Window::Close()
{
    glfwSetWindowShouldClose(glfw_window_, 1);
}

void Window::ResizeCallback(GLFWwindow* /*window*/, const int width, const int height)
{
    width_ = width;
    height_ = height;
    glViewport(0, 0, width, height);
}

void Window::GLFWErrorCallback(int error, const char* description)
{
    std::cerr << "Error(" << error << "): " << description << std::endl;
}
} // namespace Soldat
