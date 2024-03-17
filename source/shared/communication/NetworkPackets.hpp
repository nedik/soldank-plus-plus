#ifndef __NETWORK_PACKETS_HPP__
#define __NETWORK_PACKETS_HPP__

#include "core/state/Control.hpp"
#include "core/animations/Animation.hpp"

#include <cstdint>

namespace Soldat
{
#pragma pack(push, 1)
struct SoldierInputPacket
{
    unsigned int input_sequence_id;
    unsigned int game_tick;
    unsigned int player_id;
    float position_x;
    float position_y;
    float mouse_position_x;
    float mouse_position_y;
    Control control;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SoldierStatePacket
{
    unsigned int game_tick;
    unsigned int player_id;
    float position_x;
    float position_y;
    float old_position_x;
    float old_position_y;
    AnimationType body_animation_type;
    unsigned int body_animation_frame;
    int body_animation_speed;
    AnimationType legs_animation_type;
    unsigned int legs_animation_frame;
    int legs_animation_speed;
    float velocity_x;
    float velocity_y;
    float force_x;
    float force_y;
    bool on_ground;
    bool on_ground_for_law;
    bool on_ground_last_frame;
    bool on_ground_permanent;
    std::uint8_t stance;
    float mouse_position_x;
    float mouse_position_y;
    bool using_jets;
    std::int32_t jets_count;
    unsigned int last_processed_input_id;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SoldierInfoPacket
{
    unsigned int soldier_id;
};
#pragma pack(pop)
} // namespace Soldat

#endif
