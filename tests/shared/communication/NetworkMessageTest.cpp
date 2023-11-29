#include "communication/NetworkMessage.hpp"
#include "communication/NetworkEvent.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <array>

using namespace Soldat;

TEST(NetworkMessageTests, TestNetworkMessageConstructorOneString)
{
    std::vector<unsigned char> expected_bytes{ 1, 0, 0, 0, 'T', 'e', 's', 't' };

    NetworkMessage network_message(
      NetworkEvent::AssignPlayerId, "Test", expected_bytes.size() - sizeof(NetworkEvent));
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

    NetworkMessage network_message(NetworkEvent::AssignPlayerId, test_struct, sizeof(test_struct));
    auto data = network_message.GetData();

    ASSERT_EQ(data.size(), expected_bytes.size());
    for (unsigned int i = 0; i < expected_bytes.size(); i++) {
        ASSERT_EQ(expected_bytes[i], data[i]);
    }
}

TEST(NetworkMessageTests, TestNetworkMessageConstructorManyStrings)
{
    std::vector<unsigned char> expected_bytes{ 1, 0, 0, 0, 'T', 'e', 's', 't', '4', '9' };

    NetworkMessage network_message(NetworkEvent::AssignPlayerId, "Test", 4, "49", 2);
    auto data = network_message.GetData();

    ASSERT_EQ(data.size(), expected_bytes.size());
    for (unsigned int i = 0; i < expected_bytes.size(); i++) {
        ASSERT_EQ(expected_bytes[i], data[i]);
    }
}

TEST(NetworkMessageTests, TestNetworkMessageConstructorManyVariables)
{
    std::vector<unsigned char> expected_bytes{ 1, 0, 0, 0, 'T', 'e', 's', 't', 5, 0, 0, 0, 6, 0 };

    int variable_2 = 5;
    short variable_3 = 6;
    NetworkMessage network_message(NetworkEvent::AssignPlayerId,
                                   "Test",
                                   4,
                                   variable_2,
                                   sizeof(variable_2),
                                   variable_3,
                                   sizeof(variable_3));
    auto data = network_message.GetData();

    ASSERT_EQ(data.size(), expected_bytes.size());
    for (unsigned int i = 0; i < expected_bytes.size(); i++) {
        ASSERT_EQ(expected_bytes[i], data[i]);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}