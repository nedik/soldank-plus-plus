#ifndef __CLIENT_STATE_HPP__
#define __CLIENT_STATE_HPP__

#include "core/math/Glm.hpp"

#include "communication/NetworkPackets.hpp"

#include <optional>
#include <list>
#include <chrono>
#include <cstdint>

namespace Soldat
{
struct ClientState
{
    /**
     * This is nullopt when the soldier is not created yet.
     * For example when client is still in a connecting state
     * with the server and server didn't create the Soldier
     * for the client
     */
    std::optional<unsigned int> client_soldier_id;

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

    std::optional<std::chrono::time_point<std::chrono::system_clock>> last_ping_check_time;
    std::uint16_t last_ping;
    int network_lag;
};
} // namespace Soldat

#endif
