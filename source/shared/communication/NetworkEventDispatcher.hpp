#ifndef __NETWORK_EVENT_DISPATCHER_HPP__
#define __NETWORK_EVENT_DISPATCHER_HPP__

#include "communication/NetworkEvent.hpp"
#include "communication/NetworkMessage.hpp"

#include "core/math/Glm.hpp"
#include "core/state/Control.hpp"
#include "core/animations/Animation.hpp"
#include "core/types/BulletType.hpp"
#include "core/types/TeamType.hpp"
#include "core/types/WeaponType.hpp"

#include <memory>
#include <optional>
#include <variant>
#include <utility>
#include <functional>
#include <vector>

namespace Soldat
{
enum class NetworkEventDispatchResult
{
    Success = 0,
    ParseError,
    ObserverFailure,
    HandlerFailure
};

enum class NetworkEventHandlerResult
{
    Success = 0,
    Failure = 1,
};

struct ConnectionMetadata
{
    unsigned int connection_id;
    std::function<void(const NetworkMessage&)> send_message_to_connection;
};

class INetworkEventHandler
{
public:
    virtual ~INetworkEventHandler() = default;
    virtual bool ShouldHandleNetworkEvent(NetworkEvent network_event) const = 0;
    virtual std::optional<ParseError> ValidateNetworkMessage(
      const NetworkMessage& network_message) const = 0;
    virtual NetworkEventHandlerResult HandleNetworkMessage(
      unsigned int sender_connection_id,
      const NetworkMessage& network_message) = 0;
};

template<typename... NetworkMessageArgs>
class NetworkEventHandlerBase : public INetworkEventHandler
{
public:
    bool ShouldHandleNetworkEvent(NetworkEvent network_event) const override
    {
        return network_event == GetTargetNetworkEvent();
    }

    std::optional<ParseError> ValidateNetworkMessage(
      const NetworkMessage& network_message) const override
    {
        auto parsed = GetNetworkMessageOrError(network_message);
        if (!parsed.has_value()) {
            return parsed.error();
        }

        return std::nullopt;
    }

    NetworkEventHandlerResult HandleNetworkMessage(unsigned int sender_connection_id,
                                                   const NetworkMessage& network_message) override
    {
        auto parsed = GetNetworkMessageOrError(network_message);
        if (!parsed.has_value()) {
            // Before using this method, the network_message should be validated. Here we just
            // assume the network_message is correct
            return NetworkEventHandlerResult::Failure;
        }

        return std::apply(
          [this, sender_connection_id](NetworkEvent /*ignore*/,
                                       NetworkMessageArgs... network_message_args) {
              return HandleNetworkMessageImpl(sender_connection_id, network_message_args...);
          },
          *parsed);
    }

protected:
    std::expected<std::tuple<NetworkEvent, NetworkMessageArgs...>, ParseError>
    GetNetworkMessageOrError(const NetworkMessage& network_message) const
    {
        return network_message.Parse<NetworkEvent, NetworkMessageArgs...>();
    }

    virtual NetworkEvent GetTargetNetworkEvent() const = 0;
    virtual NetworkEventHandlerResult HandleNetworkMessageImpl(unsigned int sender_connection_id,
                                                               NetworkMessageArgs...) = 0;
};

class NetworkEventDispatcher
{

public:
    using TDispatchResult =
      std::pair<NetworkEventDispatchResult, std::variant<ParseError, NetworkEventHandlerResult>>;

    NetworkEventDispatcher(
      const std::vector<std::shared_ptr<INetworkEventHandler>>& network_event_handlers);

    TDispatchResult ProcessNetworkMessage(const ConnectionMetadata& connection_metadata,
                                          const NetworkMessage& network_message);

    void AddNetworkEventHandler(const std::shared_ptr<INetworkEventHandler>& network_event_handler);

private:
    std::vector<std::shared_ptr<INetworkEventHandler>> network_event_handlers_;
};
} // namespace Soldat

#endif
