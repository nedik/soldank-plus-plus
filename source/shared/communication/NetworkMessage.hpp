#ifndef __NETWORK_MESSAGE_HPP__
#define __NETWORK_MESSAGE_HPP__

#include "communication/NetworkEvent.hpp"

#include <span>
#include <vector>
#include <string>

namespace Soldat
{
class NetworkMessage
{
public:
    template<class Arg>
    NetworkMessage(NetworkEvent event, Arg one_arg, unsigned int size)
    {
        auto event_value = std::to_underlying(event);
        auto event_value_in_bytes = std::span<char>(
          static_cast<char*>(static_cast<void*>(&event_value)), sizeof(event_value));
        data_.append_range(event_value_in_bytes);
        AppendBytes(one_arg, size);
    }

    template<class... Args>
    NetworkMessage(NetworkEvent event, Args... args)
    {
        auto event_value = std::to_underlying(event);
        auto event_value_in_bytes = std::span<char>(
          static_cast<char*>(static_cast<void*>(&event_value)), sizeof(event_value));
        data_.append_range(event_value_in_bytes);
        AppendBytes(args...);
    }

    template<class Head, class... Tail>
    void AppendBytes(Head head, Tail... tail)
    {
        AppendBytes(head);
        AppendBytes(tail...);
    }

    void AppendBytes(const std::string& text)
    {
        unsigned short text_length = text.length();
        data_.append_range(std::span{ static_cast<const char*>(static_cast<void*>(&text_length)),
                                      sizeof(text_length) });
        data_.append_range(std::span{
          static_cast<const char*>(static_cast<const void*>(text.c_str())), text_length });
    }

    void AppendBytes(const char* head)
    {
        std::string text = head;
        unsigned short text_length = text.length();
        data_.append_range(std::span{ static_cast<const char*>(static_cast<void*>(&text_length)),
                                      sizeof(text_length) });
        data_.append_range(std::span{
          static_cast<const char*>(static_cast<const void*>(text.c_str())), text_length });
    }

    template<typename Head>
    void AppendBytes(Head head)
    {
        data_.append_range(
          std::span{ static_cast<const char*>(static_cast<void*>(&head)), sizeof(Head) });
    }

    std::span<const char> GetData() const { return { data_ }; }

private:
    std::vector<char> data_;
};
}; // namespace Soldat

#endif
