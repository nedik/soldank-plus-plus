#ifndef __CLIENT_STATE_HPP__
#define __CLIENT_STATE_HPP__

#include "core/math/Glm.hpp"

#include "communication/NetworkPackets.hpp"
#include "communication/PingTimer.hpp"

#include <optional>
#include <list>
#include <chrono>
#include <cstdint>

namespace Soldank
{
struct ClientState
{
    /**
     * This is nullopt when the soldier is not created yet.
     * For example when client is still in a connecting state
     * with the server and server didn't create the Soldier
     * for the client
     */
    std::optional<std::uint8_t> client_soldier_id;

    glm::vec2 camera;
    glm::vec2 camera_prev;
    glm::vec2 mouse;
    bool mouse_left_button_pressed;
    bool mouse_right_button_pressed;
    float game_width;
    float game_height;

    glm::vec2 soldier_position_server_pov;

    std::list<SoldierInputPacket> pending_inputs;
    bool server_reconciliation;
    bool client_side_prediction;
    bool objects_interpolation;
    bool draw_server_pov_client_pos;

    int network_lag;

    PingTimer ping_timer;

    WeaponType primary_weapon_type_choice = WeaponType::DesertEagles;
    WeaponType secondary_weapon_type_choice = WeaponType::USSOCOM;

    bool kill_button_just_pressed = false;
};
} // namespace Soldank

#endif
