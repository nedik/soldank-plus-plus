#include <utility>

#include "communication/NetworkEvent.hpp"
#include "communication/NetworkEventDispatcher.hpp"
#include "communication/NetworkMessage.hpp"
#include "communication/NetworkPackets.hpp"

#include "core/math/Glm.hpp"

#include "spdlog/spdlog.h"

namespace Soldank
{
NetworkEventDispatcher::NetworkEventDispatcher(
  const std::vector<std::shared_ptr<INetworkEventHandler>>& network_event_handlers)
    : network_event_handlers_(network_event_handlers)
{
}

NetworkEventDispatcher::TDispatchResult NetworkEventDispatcher::ProcessNetworkMessage(
  const ConnectionMetadata& connection_metadata,
  const NetworkMessage& network_message)
{
    auto network_event_or_error = network_message.GetNetworkEvent();
    if (!network_event_or_error.has_value()) {
        return { NetworkEventDispatchResult::ParseError, network_event_or_error.error() };
    }

    auto network_event = *network_event_or_error;

    for (const auto& network_event_handler : network_event_handlers_) {
        if (!network_event_handler->ShouldHandleNetworkEvent(network_event)) {
            continue;
        }

        auto parse_error_or_nothing =
          network_event_handler->ValidateNetworkMessage(network_message);
        if (parse_error_or_nothing.has_value()) {
            return { NetworkEventDispatchResult::ParseError, parse_error_or_nothing.value() };
        }

        auto handler_result = network_event_handler->HandleNetworkMessage(
          connection_metadata.connection_id, network_message);
        switch (handler_result) {
            case NetworkEventHandlerResult::Success:
                return { NetworkEventDispatchResult::Success, handler_result };
            case NetworkEventHandlerResult::Failure:
                return { NetworkEventDispatchResult::HandlerFailure, handler_result };
        }
    }

    return { NetworkEventDispatchResult::ParseError, ParseError::InvalidNetworkEvent };
}

void NetworkEventDispatcher::AddNetworkEventHandler(
  const std::shared_ptr<INetworkEventHandler>& network_event_handler)
{
    network_event_handlers_.push_back(network_event_handler);
}
} // namespace Soldank
