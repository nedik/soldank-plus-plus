#ifdef _WIN32
#include <windows.h>
#endif

#include "application/Application.hpp"

#include <exception>
#include <iostream>

// TODO: odkomentować
// #ifdef _WIN32
// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
// #else
int main(int /*argc*/, char* /*argv*/[])
// #endif
{
    Soldat::Application::Init();
    Soldat::Application::Run();
    Soldat::Application::Free();

    return 0;
}
