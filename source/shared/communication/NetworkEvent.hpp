#ifndef __NETWORK_EVENTS_HPP__
#define __NETWORK_EVENTS_HPP__

namespace Soldat
{
enum class NetworkEvent : unsigned int
{
    ChatMessage = 0,
    AssignPlayerId,
    SpawnSoldier,
    SoldierInput
};
}

#endif
