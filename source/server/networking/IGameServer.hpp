#ifndef __IGAME_SERVER_HPP__
#define __IGAME_SERVER_HPP__

#include "communication/NetworkMessage.hpp"

namespace Soldat
{
class IGameServer
{
public:
    virtual ~IGameServer() = default;
    virtual void Update() = 0;

    virtual void SendNetworkMessage(unsigned int connection_id,
                                    const NetworkMessage& network_message) = 0;
};
} // namespace Soldat

#endif