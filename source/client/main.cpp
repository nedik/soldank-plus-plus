#ifdef _WIN32
#include <windows.h>
#endif

#include "application/Application.hpp"

#include <exception>

// TODO: odkomentować
// #ifdef _WIN32
// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
// #else
int main(int argc, const char* argv[])
// #endif
{
    if (!Soldank::Application::Init(argc, argv)) {
        return 1;
    }
    Soldank::Application::Run();
    Soldank::Application::Free();

    return 0;
}
