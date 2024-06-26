#include "Window.hpp"

#include "application/config/Config.hpp"
#include "application/input/Keyboard.hpp"
#include "application/input/Mouse.hpp"
#include "application/window/Window.hpp"

#include "spdlog/spdlog.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

namespace Soldank
{
Window::Window()
    : Window("Soldank++", Config::INITIAL_WINDOW_WIDTH, Config::INITIAL_WINDOWS_HEIGHT)
{
}

Window::Window(std::string title, int width, int height)
    : title_(std::move(title))
    , glfw_window_(nullptr)
{
    if (glfwInit() == 0) {
        spdlog::error("Error: Failed to initialize GLFW");
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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}

void Window::Create()
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (monitor == nullptr) {
        spdlog::error("Error: Failed to get primary monitor");
        throw "Error: Failed to get primary monitor";
    }

    glfw_window_ =
      glfwCreateWindow(width_, height_, title_.c_str(), /* monitor */ nullptr, nullptr);
    if (nullptr == glfw_window_) {
        spdlog::error("Error: Failed to create window.");
        throw "Error: Failed to create window.";
    }

    glfwMakeContextCurrent(glfw_window_);

    glfwSwapInterval(0);

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0) {
        spdlog::error("Error: Failed to initialize GLAD.");
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForOpenGL(glfw_window_, true);
    ImGui_ImplOpenGL3_Init();
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
    spdlog::error("Error({}): {}", error, description);
}
} // namespace Soldank
