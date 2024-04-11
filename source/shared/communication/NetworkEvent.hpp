#ifndef __NETWORK_EVENTS_HPP__
#define __NETWORK_EVENTS_HPP__

namespace Soldank
{
enum class NetworkEvent : unsigned int
{
    ChatMessage = 0,
    AssignPlayerId,
    SpawnSoldier,
    SoldierInput,
    SoldierState,
    SoldierInfo,
    PlayerLeave,
    PingCheck,
    ProjectileSpawn,
    KillCommand,
    KillSoldier,
    HitSoldier
};
}

#endif
