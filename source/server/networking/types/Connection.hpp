#ifndef __CONNECTION_HPP__
#define __CONNECTION_HPP__

#include <steam/steamnetworkingsockets.h>

#include <string>

namespace Soldat
{
struct Connection
{
    HSteamNetConnection connection_handle;
    std::string nick;
};
} // namespace Soldat

#endif
