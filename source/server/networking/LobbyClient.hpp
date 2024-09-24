#ifndef __LOBBY_CLIENT_HPP__
#define __LOBBY_CLIENT_HPP__

#include "httplib.h"

#include <cstdint>
#include <string>
#include <vector>

namespace Soldank
{
class LobbyClient
{
public:
    LobbyClient();

    void Register(const std::string& server_name, std::uint16_t server_port);

private:
    struct HTTPClient
    {
        httplib::Client sender;
        std::string api_endpoint;
    };

    std::vector<HTTPClient> http_clients_;
};
} // namespace Soldank

#endif
