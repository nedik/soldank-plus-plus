#include "communication/NetworkMessage.hpp"
#include "communication/NetworkEvent.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <array>

using namespace Soldat;

TEST(NetworkMessageTests, TestNetworkMessageConstructorOneString)
{
    std::vector<unsigned char> expected_bytes{ 1, 0, 0, 0, 4, 0, 'T', 'e', 's', 't' };

    NetworkMessage network_message(NetworkEvent::AssignPlayerId, "Test");
    auto data = network_message.GetData();

    ASSERT_EQ(data.size(), expected_bytes.size());
    for (unsigned int i = 0; i < expected_bytes.size(); i++) {
        ASSERT_EQ(expected_bytes[i], data[i]);
    }
}

#pragma pack(push, 1)
struct TestStruct
{
    char a;
    int b;
};
#pragma pack(pop)

TEST(NetworkMessageTests, TestNetworkMessageConstructorOneStruct)
{
    std::vector<unsigned char> expected_bytes{ 1, 0, 0, 0, 't', 4, 0, 0, 0 };

    TestStruct test_struct{ 't', 4 };

    NetworkMessage network_message(NetworkEvent::AssignPlayerId, test_struct);
    auto data = network_message.GetData();

    ASSERT_EQ(data.size(), expected_bytes.size());
    for (unsigned int i = 0; i < expected_bytes.size(); i++) {
        ASSERT_EQ(expected_bytes[i], data[i]);
    }
}

TEST(NetworkMessageTests, TestNetworkMessageConstructorManyStrings)
{
    std::vector<unsigned char> expected_bytes{
        1, 0, 0, 0, 4, 0, 'T', 'e', 's', 't', 2, 0, '4', '9'
    };

    NetworkMessage network_message(NetworkEvent::AssignPlayerId, "Test", "49");
    auto data = network_message.GetData();

    ASSERT_EQ(data.size(), expected_bytes.size());
    for (unsigned int i = 0; i < expected_bytes.size(); i++) {
        ASSERT_EQ(expected_bytes[i], data[i]);
    }
}

TEST(NetworkMessageTests, TestNetworkMessageConstructorManyVariables)
{
    std::vector<unsigned char> expected_bytes{ 1,   0,   0, 0, 4, 0, 'T', 'e',
                                               's', 't', 5, 0, 0, 0, 6,   0 };

    int variable_2 = 5;
    short variable_3 = 6;
    NetworkMessage network_message(NetworkEvent::AssignPlayerId, "Test", variable_2, variable_3);
    auto data = network_message.GetData();

    ASSERT_EQ(data.size(), expected_bytes.size());
    for (unsigned int i = 0; i < expected_bytes.size(); i++) {
        ASSERT_EQ(expected_bytes[i], data[i]);
    }
}

TEST(NetworkMessageTests, TestNetworkMessageGetParameters)
{
    int expected_variable_1 = 5;
    int expected_variable_2 = 6;
    NetworkMessage network_message(
      NetworkEvent::AssignPlayerId, expected_variable_1, expected_variable_2, "Test", 5);
    auto data = network_message.GetData();

    auto [network_event, variable_1, variable_2, text, variable_3] =
      NetworkMessage::GetParameters<NetworkEvent, int, int, std::string, int>(data);

    ASSERT_EQ(network_event, NetworkEvent::AssignPlayerId);
    ASSERT_EQ(variable_1, expected_variable_1);
    ASSERT_EQ(variable_2, expected_variable_2);
    ASSERT_EQ(text, "Test");
    ASSERT_EQ(variable_3, 5);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}