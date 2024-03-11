#ifndef __SERVER_STATE_HPP__
#define __SERVER_STATE_HPP__

#include "core/config/Config.hpp"

#include <array>

namespace Soldat
{
struct ServerState
{
    std::array<unsigned int, Config::MAX_PLAYERS> last_processed_input_id;
};
} // namespace Soldat

#endif
