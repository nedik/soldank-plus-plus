#ifndef __PLAYER_INPUT_HPP__
#define __PLAYER_INPUT_HPP__

#include <memory>

namespace Soldank
{
struct Soldier;
struct Control;
struct ClientState;
} // namespace Soldank

namespace Soldank::PlayerInput
{
void UpdatePlayerSoldierControlCollisions(const Soldier& soldier,
                                          Control& soldier_control,
                                          const std::shared_ptr<ClientState>& client_state);
} // namespace Soldank::PlayerInput

#endif
