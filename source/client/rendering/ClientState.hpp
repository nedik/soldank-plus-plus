#ifndef __CLIENT_STATE_HPP__
#define __CLIENT_STATE_HPP__

#include "core/math/Glm.hpp"

#include <optional>

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
};
} // namespace Soldat

#endif
