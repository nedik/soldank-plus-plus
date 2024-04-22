#ifndef __STATE_MANAGER_HPP__
#define __STATE_MANAGER_HPP__

#include "core/state/State.hpp"
#include "core/state/Control.hpp"

#include <memory>

namespace Soldank
{
class StateManager
{
public:
    State& GetState() { return state_; }

    void ChangeSoldierControlActionState(std::uint8_t soldier_id,
                                         ControlActionType control_action_type,
                                         bool new_state);

private:
    Soldier& GetSoldierRef(std::uint8_t soldier_id);

    State state_;
};
} // namespace Soldank

#endif
