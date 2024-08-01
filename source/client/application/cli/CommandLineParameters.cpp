#include "application/cli/CommandLineParameters.hpp"

#include "spdlog/spdlog.h"

#include <cxxopts.hpp>

#include <iostream>
#include <cstdint>

namespace Soldank::CommandLineParameters
{
// NOLINTNEXTLINE modernize-avoid-c-arrays
ParsedValues Parse(int argc, const char* argv[])
{
    ParsedValues parsed_values;

    try {
        std::unique_ptr<cxxopts::Options> allocated(
          // NOLINTNEXTLINE cppcoreguidelines-pro-bounds-pointer-arithmetic
          new cxxopts::Options(argv[0], "Soldank++ client"));
        auto& options = *allocated;
        options.positional_help("[optional args]").show_positional_help();

        // clang-format off
        options
            .set_width(80)
            .add_options()
            ("help", "Print help")
            ("l,local", "Start local game")
            ("o,online",
                "Start online game. You need to provide IP and Port with --ip and --port options")
            ("ip", "IP of the server to join", cxxopts::value<std::string>())
            ("port", "Port of the server to join", cxxopts::value<std::uint8_t>());
        // clang-format on

        auto result = options.parse(argc, argv);

        if (result.count("help") != 0) {
            std::cout << options.help({ "", "Group" }) << std::endl;
            return parsed_values;
        }

        if (result.count("local") != 0) {
            std::cout << "local" << std::endl;
            parsed_values.is_online = false;
        }

        if (result.count("online") != 0) {
            std::cout << "online" << std::endl;
            parsed_values.is_online = true;
        }

        parsed_values.is_parsing_successful = true;
        return parsed_values;
    } catch (const cxxopts::exceptions::exception& e) {
        spdlog::critical("error parsing options {}", e.what());
        return parsed_values;
    }
    return parsed_values;
}
} // namespace Soldank::CommandLineParameters
