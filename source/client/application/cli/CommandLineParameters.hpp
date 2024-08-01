#ifndef __COMMAND_LINE_PARAMETERS_HPP__
#define __COMMAND_LINE_PARAMETERS_HPP__

#include <optional>

namespace Soldank::CommandLineParameters
{
struct ParsedValues
{
    bool is_parsing_successful = false;
    std::optional<bool> is_online = std::nullopt;
};

// NOLINTNEXTLINE modernize-avoid-c-arrays
ParsedValues Parse(int argc, const char* argv[]);
} // namespace Soldank::CommandLineParameters

#endif
