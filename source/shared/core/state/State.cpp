#include "core/state/State.hpp"

namespace Soldat
{
State::State(const std::string& map_name)
    : map(map_name)
    , gravity(0.06)
{
}
} // namespace Soldat
