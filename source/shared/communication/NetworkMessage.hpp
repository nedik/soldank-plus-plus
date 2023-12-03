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
enum class ParseError : unsigned short
{
    InvalidStringSize = 0,
    BufferTooSmall,
    BufferTooBig,
    InvalidString,
};

namespace
{
template<unsigned int N>
struct NetworkMessageData
{
    template<typename Arg>
    static std::expected<Arg, ParseError> ParseDataParameter(std::span<const char> data)
    {
        if (data.size() < sizeof(Arg)) {
            return std::unexpected(ParseError::BufferTooSmall);
        }

        Arg converted_data =
          *static_cast<const Arg*>(static_cast<const void*>(data.subspan(0, sizeof(Arg)).data()));
        return converted_data;
    }

    template<>
    static std::expected<std::string, ParseError> ParseDataParameter<std::string>(
      std::span<const char> data)
    {
        auto text_size_or_error =
          ParseDataParameter<unsigned short>(data.subspan(0, sizeof(unsigned short)));
        if (!text_size_or_error.has_value()) {
            return std::unexpected(text_size_or_error.error());
        }

        unsigned short text_size = *text_size_or_error;
        if (text_size == 0 || text_size > data.size() - 2) {
            return std::unexpected(ParseError::InvalidStringSize);
        }

        for (char character : data.subspan(2, text_size)) {
            if (character == 0) {
                return std::unexpected(ParseError::InvalidString);
            }
        }

        auto text_data = data.subspan(2, text_size);
        std::string text{ text_data.begin(), text_data.end() };
        return text;
    }

    template<typename Arg>
    static std::expected<unsigned int, ParseError> ParseDataParameterSize(
      std::span<const char> data)
    {
        if (data.size() < sizeof(Arg)) {
            return std::unexpected(ParseError::BufferTooSmall);
        }

        return sizeof(Arg);
    }

    template<>
    static std::expected<unsigned int, ParseError> ParseDataParameterSize<std::string>(
      std::span<const char> data)
    {
        auto text_size_or_error =
          ParseDataParameter<unsigned short>(data.subspan(0, sizeof(unsigned short)));
        if (!text_size_or_error.has_value()) {
            return std::unexpected(text_size_or_error.error());
        }
        unsigned short text_size = *text_size_or_error;
        if ((text_size == 0) || text_size > data.size() - 2) {
            return std::unexpected(ParseError::InvalidStringSize);
        }

        return sizeof(unsigned short) + text_size;
    }

    template<typename Head, typename... Tail>
    static std::expected<std::tuple<Head, Tail...>, ParseError> ParseData(
      std::span<const char> data)
    {
        auto head_or_error = ParseDataParameter<Head>(data);
        if (!head_or_error.has_value()) {
            return std::unexpected(head_or_error.error());
        }

        auto tail_offset_or_error = ParseDataParameterSize<Head>(data);
        if (!tail_offset_or_error.has_value()) {
            return std::unexpected(tail_offset_or_error.error());
        }

        auto tail_or_error = NetworkMessageData<N - 1>::template ParseData<Tail...>(
          data.subspan(*tail_offset_or_error));
        if (!tail_or_error.has_value()) {
            return std::unexpected(tail_or_error.error());
        }
        std::tuple<Head> head_in_tuple{ *head_or_error };
        return std::tuple_cat(head_in_tuple, *tail_or_error);
    }
};

template<>
struct NetworkMessageData<1>
{
    template<typename Arg>
    static std::expected<std::tuple<Arg>, ParseError> ParseData(std::span<const char> data)
    {
        auto last_parameter_size_or_error =
          NetworkMessageData<2>::ParseDataParameterSize<Arg>(data);
        if (!last_parameter_size_or_error.has_value()) {
            return std::unexpected(last_parameter_size_or_error.error());
        }
        auto last_parameter_size = *last_parameter_size_or_error;
        if (data.size() > last_parameter_size) {
            return std::unexpected(ParseError::BufferTooBig);
        }

        auto parsed_or_error = NetworkMessageData<2>::ParseDataParameter<Arg>(data);
        if (!parsed_or_error.has_value()) {
            return std::unexpected(parsed_or_error.error());
        }

        return { *parsed_or_error };
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
    static std::expected<std::tuple<Args...>, ParseError> ParseData(std::span<const char> data)
    {
        return NetworkMessageData<sizeof...(Args)>::template ParseData<Args...>(data);
    }

private:
    std::vector<char> data_;
};
}; // namespace Soldat

#endif
