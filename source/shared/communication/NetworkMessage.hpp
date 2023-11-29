#ifndef __NETWORK_MESSAGE_HPP__
#define __NETWORK_MESSAGE_HPP__

#include "communication/NetworkEvent.hpp"

#include <span>
#include <vector>

namespace Soldat
{
class NetworkMessage
{
public:
    template<class Arg>
    NetworkMessage(NetworkEvent event, Arg one_arg, unsigned int size)
    {
        auto event_value = std::to_underlying(event);
        auto event_value_in_bytes = std::span<unsigned char>(
          static_cast<unsigned char*>(static_cast<void*>(&event_value)), sizeof(event_value));
        data_.append_range(event_value_in_bytes);
        AppendBytes(one_arg, size);
    }

    template<class... Args>
    NetworkMessage(NetworkEvent event, Args... args)
    {
        auto event_value = std::to_underlying(event);
        auto event_value_in_bytes = std::span<unsigned char>(
          static_cast<unsigned char*>(static_cast<void*>(&event_value)), sizeof(event_value));
        data_.append_range(event_value_in_bytes);
        AppendBytes(args...);
    }

    template<class Head, class... Tail>
    void AppendBytes(Head head, unsigned int size, Tail... tail)
    {
        AppendBytes(head, size);
        AppendBytes(tail...);
    }

    void AppendBytes(const char* head, unsigned int size)
    {
        data_.append_range(
          std::span{ static_cast<const unsigned char*>(static_cast<const void*>(head)), size });
    }

    void AppendBytes(const unsigned char* head, unsigned int size)
    {
        data_.append_range(
          std::span{ static_cast<const unsigned char*>(static_cast<const void*>(head)), size });
    }

    template<typename Head>
    void AppendBytes(Head head, unsigned int size)
    {
        data_.append_range(
          std::span{ static_cast<const unsigned char*>(static_cast<void*>(&head)), size });
    }

    std::span<const unsigned char> GetData() const { return { data_ }; }

private:
    std::vector<unsigned char> data_;
};
}; // namespace Soldat

#endif
