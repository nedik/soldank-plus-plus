#include "core/state/State.hpp"

namespace Soldat
{
State::State(const std::string& map_name)
    : map(map_name)
    , gravity(0.06)
    , camera(0.0, 0.0)
    , mouse(0.0, 0.0)
    , game_width(640.0)
    , game_height(480.0)
{
}
} // namespace Soldat
