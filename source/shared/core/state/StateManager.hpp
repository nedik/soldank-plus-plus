#ifndef __STATE_MANAGER_HPP__
#define __STATE_MANAGER_HPP__

#include "core/state/State.hpp"

#include <memory>

namespace Soldank
{
class StateManager
{
public:
    State& GetState() { return state_; }

private:
    State state_;
};
} // namespace Soldank

#endif
