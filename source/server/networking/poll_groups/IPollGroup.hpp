#ifndef __I_POLL_GROUP_HPP__
#define __I_POLL_GROUP_HPP__

class ISteamNetworkingSockets;
struct SteamNetConnectionStatusChangedCallback_t;

namespace Soldat
{
class IPollGroup
{
public:
    virtual ~IPollGroup() = default;

    IPollGroup(IPollGroup&& other) = delete;
    IPollGroup& operator=(IPollGroup&& other) = delete;
    IPollGroup(IPollGroup& other) = delete;
    IPollGroup& operator=(IPollGroup& other) = delete;

    virtual void PollIncomingMessages() = 0;
    virtual void AcceptConnection(
      SteamNetConnectionStatusChangedCallback_t* new_connection_info) = 0;
    virtual void CloseConnection(SteamNetConnectionStatusChangedCallback_t* connection_info) = 0;

protected:
    IPollGroup(ISteamNetworkingSockets* interface)
        : interface_(interface)
    {
    }

    ISteamNetworkingSockets* GetInterface() const { return interface_; }

private:
    ISteamNetworkingSockets* interface_;
};
} // namespace Soldat

#endif
