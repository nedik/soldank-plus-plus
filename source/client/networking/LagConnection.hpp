#ifndef __LAG_CONNECTION_HPP__
#define __LAG_CONNECTION_HPP__

#include "networking/IConnection.hpp"

#include "communication/NetworkEventDispatcher.hpp"

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#include <memory>
#include <list>
#include <chrono>

namespace Soldat
{
class LagConnection : public IConnection
{
public:
    LagConnection(ISteamNetworkingSockets* interface, HSteamNetConnection connection_handle);

    void PollIncomingMessages(
      const std::shared_ptr<NetworkEventDispatcher>& network_event_dispatcher) override;

    void CloseConnection() override;

    void AssertConnectionInfo(
      SteamNetConnectionStatusChangedCallback_t* connection_info) const override;

    void SendNetworkMessage(const NetworkMessage& network_message) override;

    void SetLag(unsigned int milliseconds);

private:
    struct MessageToReceive
    {
        ConnectionMetadata connection_metadata;
        std::chrono::time_point<std::chrono::system_clock> when_to_receive;
        NetworkMessage received_message;
    };

    ISteamNetworkingSockets* interface_;
    HSteamNetConnection connection_handle_;

    std::chrono::duration<unsigned int, std::milli> lag_to_add_;
    std::list<std::pair<std::chrono::time_point<std::chrono::system_clock>, NetworkMessage>>
      messages_to_send_;
    std::list<MessageToReceive> messages_to_receive_;
};
} // namespace Soldat

#endif
