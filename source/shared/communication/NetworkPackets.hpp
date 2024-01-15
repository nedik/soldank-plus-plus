#ifndef __NETWORK_PACKETS_HPP__
#define __NETWORK_PACKETS_HPP__

namespace Soldat
{
#pragma pack(push, 1)
struct UpdateSoldierStatePacket
{
    unsigned int id;
    float position_x;
    float position_y;
};
#pragma pack(pop)
} // namespace Soldat

#endif
