#include "networking/LobbyClient.hpp"

#include "core/data/FileReader.hpp"

#include "spdlog/spdlog.h"
#include "nlohmann/json.hpp"

#include <utility>
#include <cctype>

using json = nlohmann::json;

namespace Soldank
{
LobbyClient::LobbyClient()
{
    std::string file_path = "lobby_servers.txt";
    FileReader file_reader;
    auto file_data = file_reader.Read("lobby_servers.txt");
    if (!file_data.has_value()) {
        spdlog::critical("Could not open lobby servers file: {}", file_path);
    } else {
        std::stringstream data_buffer{ *file_data };
        std::string protocol;
        std::string scheme_host_port;
        std::string api_endpoint;
        while (data_buffer >> protocol >> scheme_host_port >> api_endpoint) {
            std::transform(protocol.begin(), protocol.end(), protocol.begin(), [](unsigned char c) {
                return std::tolower(c);
            });
            if (api_endpoint.ends_with("/")) {
                api_endpoint.pop_back();
            }
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
            if (protocol == "https") {
                protocol = "https://";
            } else {
#endif
                protocol = "http://";
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
            }
#endif
            http_clients_.push_back(
              { httplib::Client(std::string{ protocol }.append(scheme_host_port)), api_endpoint });
            http_clients_.back().sender.set_follow_location(true);
            http_clients_.back().sender.set_keep_alive(false);
        }
    }
}

void LobbyClient::Register(const std::string& server_name, std::uint16_t server_port)
{
#ifdef _WIN32
    std::string operating_system = "Windows";
#elif defined __linux__
    std::string operating_system = "Linux";
#elif defined __APPLE__
    std::string operating_system = "MacOS";
#elif defined __unix__
    std::string operating_system = "UNIX";
#else
    std::string operating_system = "UNKNOWN";
#endif

    // TODO: populate with proper settings
    json server_info = {
        { "advanced", false },   { "anti_cheat_on", false },   { "bonus_frequency", 0 },
        { "country", "PL" },     { "current_map", "ctf_Ash" }, { "game_style", "CTF" },
        { "info", "Test info" }, { "max_players", 12 },        { "name", server_name },
        { "num_bots", 1 },       { "os", operating_system },   { "players", json::array() },
        { "port", server_port }, { "private", false },         { "realistic", false },
        { "respawn", 180 },      { "survival", false },        { "version", "1" },
        { "wm", false },
    };

    for (auto& http_client : http_clients_) {
        auto response = http_client.sender.Post(
          http_client.api_endpoint + "/servers", server_info.dump(), "application/json");
        if (response) {
            if (response->status == 201) {
                spdlog::info("Registering in the lobby: {}", http_client.sender.host());
            } else {
                spdlog::error("Failed registering in the lobby: {}. Returned status = {}",
                              http_client.sender.host(),
                              response->status);
            }
        } else {
            spdlog::error("Could not register the server in the lobby. Error ({}): {}",
                          std::to_underlying(response.error()),
                          httplib::to_string(response.error()));
        }
    }
}
} // namespace Soldank
