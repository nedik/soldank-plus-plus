#ifndef __NETWORK_MESSAGE_HPP__
#define __NETWORK_MESSAGE_HPP__

#include "communication/NetworkEvent.hpp"

#include <span>
#include <vector>
#include <string>
#include <tuple>
#include <expected>

namespace Soldat
{
namespace
{
template<unsigned int N>
struct NetworkMessageData
{
    template<typename Arg>
    static Arg ParseDataParameter(std::span<const char> data)
    {
        Arg converted_data =
          *static_cast<const Arg*>(static_cast<const void*>(data.subspan(0, sizeof(Arg)).data()));
        return converted_data;
    }

    template<>
    static std::string ParseDataParameter<std::string>(std::span<const char> data)
    {
        unsigned short text_size =
          ParseDataParameter<unsigned short>(data.subspan(0, sizeof(unsigned short)));
        auto text_data = data.subspan(2, text_size);
        std::string text{ text_data.begin(), text_data.end() };
        return text;
    }

    template<typename Arg>
    static unsigned int ParseDataParameterSize(std::span<const char> /*data*/)
    {
        return sizeof(Arg);
    }

    template<>
    static unsigned int ParseDataParameterSize<std::string>(std::span<const char> data)
    {
        return sizeof(unsigned short) +
               ParseDataParameter<unsigned short>(data.subspan(0, sizeof(unsigned short)));
    }

    template<typename Head, typename... Tail>
    static std::tuple<Head, Tail...> ParseData(std::span<const char> data)
    {
        Head head = ParseDataParameter<Head>(data);
        std::tuple<Head> head_in_tuple{ head };
        return std::tuple_cat(head_in_tuple,
                              NetworkMessageData<N - 1>::template ParseData<Tail...>(
                                data.subspan(ParseDataParameterSize<Head>(data))));
    }
};

template<>
struct NetworkMessageData<1>
{
    template<typename Head>
    static std::tuple<Head> ParseData(std::span<const char> data)
    {
        return { NetworkMessageData<2>::ParseDataParameter<Head>(data) };
    }
};
} // namespace

class NetworkMessage
{
public:
    NetworkMessage(NetworkEvent event)
    {
        auto event_value = std::to_underlying(event);
        AppendBytes(event_value);
    }

    template<class Arg>
    NetworkMessage(NetworkEvent event, Arg one_arg, unsigned int size)
    {
        auto event_value = std::to_underlying(event);
        AppendBytes(event_value);
        AppendBytes(one_arg, size);
    }

    template<class... Args>
    NetworkMessage(NetworkEvent event, Args... args)
    {
        auto event_value = std::to_underlying(event);
        AppendBytes(event_value);
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

    template<typename... Args>
    static std::tuple<Args...> ParseData(std::span<const char> data)
    {
        return NetworkMessageData<sizeof...(Args)>::template ParseData<Args...>(data);
    }

private:
    std::vector<char> data_;
};
}; // namespace Soldat

#endif
