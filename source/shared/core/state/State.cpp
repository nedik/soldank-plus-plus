#include "core/state/State.hpp"

namespace Soldank
{
State::State(const std::string& map_name)
    : map(map_name)
    , gravity(0.06)
{
}
} // namespace Soldank
