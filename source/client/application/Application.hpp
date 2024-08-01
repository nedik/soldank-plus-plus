#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include "window/Window.hpp"

namespace Soldank::Application
{
// NOLINTNEXTLINE modernize-avoid-c-arrays
bool Init(int argc, const char* argv[]);
void Run();
void Free();
} // namespace Soldank::Application

#endif
