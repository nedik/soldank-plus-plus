#ifndef __NETWORK_PACKETS_HPP__
#define __NETWORK_PACKETS_HPP__

#include "core/state/Control.hpp"

namespace Soldat
{
#pragma pack(push, 1)
struct UpdateSoldierStatePacket
{
    unsigned int game_tick;
    unsigned int id;
    float position_x;
    float position_y;
    Control control;
};
#pragma pack(pop)
} // namespace Soldat

#endif
