#ifndef __COMMAND_LINE_PARAMETERS_HPP__
#define __COMMAND_LINE_PARAMETERS_HPP__

#include <optional>
#include <string>

namespace Soldank::CommandLineParameters
{
struct ParsedValues
{
    bool is_parsing_successful = false;
    std::optional<bool> is_online = std::nullopt;
    std::string join_server_ip;
    std::uint16_t join_server_port;
};

// NOLINTNEXTLINE modernize-avoid-c-arrays
ParsedValues Parse(int argc, const char* argv[]);
} // namespace Soldank::CommandLineParameters

#endif
