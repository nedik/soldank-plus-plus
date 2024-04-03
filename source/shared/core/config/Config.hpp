#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <cstdint>

namespace Soldank::Config
{
constexpr const char* const WEAPONS_INI_FILE_PATH = "weapons.ini";
constexpr const char* const WEAPONS_REALISTIC_INI_FILE_PATH = "weapons_realistic.ini";

constexpr std::uint8_t MAX_PLAYERS = 16;
} // namespace Soldank::Config

#endif
