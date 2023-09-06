#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

namespace Soldat::Config
{
constexpr const bool DEBUG_DRAW = false;
constexpr const int INITIAL_WINDOW_WIDTH = 1280;
constexpr const int INITIAL_WINDOWS_HEIGHT = 1024;

// If FPS_LIMIT == 0, then there's no limit
constexpr const int FPS_LIMIT = 0;

constexpr const char* const WEAPONS_INI_FILE_PATH = "weapons.ini";
constexpr const char* const WEAPONS_REALISTIC_INI_FILE_PATH = "weapons_realistic.ini";
} // namespace Soldat::Config

#endif
