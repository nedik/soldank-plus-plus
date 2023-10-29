#include "core/state/State.hpp"

namespace Soldat
{
State::State(const std::string& map_name)
    : map(map_name)
    , camera(0.0, 0.0)
    , camera_prev(0.0, 0.0)
    , mouse(0.0, 0.0)
    , game_width(640.0)
    , game_height(480.0)
    , gravity(0.06)
{
}
} // namespace Soldat
