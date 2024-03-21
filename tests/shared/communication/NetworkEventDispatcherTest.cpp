#include "communication/NetworkEventDispatcher.hpp"
#include "communication/NetworkMessage.hpp"
#include "communication/NetworkEvent.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <vector>

using namespace Soldat;

class NetworkEventObserverExample : public INetworkEventObserver
{
public:
    NetworkEventObserverResult OnAssignPlayerId(const ConnectionMetadata& /*connection_metadata*/,
                                                unsigned int assigned_player_id) override
    {
        last_assigned_player_id_ = assigned_player_id;
        return result_;
    }

    NetworkEventObserverResult OnChatMessage(const ConnectionMetadata& /*connection_metadata*/,
                                             const std::string& chat_message) override
    {
        last_chat_message_ = chat_message;
        return result_;
    }

    NetworkEventObserverResult OnSpawnSoldier(const ConnectionMetadata& /*connection_metadata*/,
                                              unsigned int /*soldier_id*/,
                                              glm::vec2 /*spawn_position*/) override
    {
        return result_;
    }

    NetworkEventObserverResult OnSoldierInput(const ConnectionMetadata& /*connection_metadata*/,
                                              unsigned int /*input_sequence_id*/,
                                              unsigned int /*soldier_id*/,
                                              glm::vec2 /*soldier_position*/,
                                              glm::vec2 /*mouse_position*/,
                                              const Control& /*player_control*/) override
    {
        return result_;
    }

    NetworkEventObserverResult OnSoldierState(const ConnectionMetadata& /*connection_metadata*/,
                                              unsigned int /*soldier_id*/,
                                              glm::vec2 /*soldier_position*/,
                                              glm::vec2 /*soldier_old_position*/,
                                              AnimationType /*body_animation_type*/,
                                              unsigned int /*body_animation_frame*/,
                                              int /*body_animation_speed*/,
                                              AnimationType /*legs_animation_type*/,
                                              unsigned int /*legs_animation_frame*/,
                                              int /*legs_animation_speed*/,
                                              glm::vec2 /*soldier_velocity*/,
                                              glm::vec2 /*soldier_force*/,
                                              bool /*on_ground*/,
                                              bool /*on_ground_for_law*/,
                                              bool /*on_ground_last_frame*/,
                                              bool /*on_ground_permanent*/,
                                              std::uint8_t /*stance*/,
                                              float /*mouse_position_x*/,
                                              float /*mouse_position_y*/,
                                              bool /*using_jets*/,
                                              std::int32_t /*jets_count*/,
                                              unsigned int /*active_weapon*/,
                                              unsigned int /*last_processed_input_id*/) override

    {
        return result_;
    }

    NetworkEventObserverResult OnSoldierInfo(const ConnectionMetadata& /*connection_metadata*/,
                                             unsigned int /*soldier_id*/) override
    {
        return result_;
    }

    NetworkEventObserverResult OnPlayerLeave(const ConnectionMetadata& /*connection_metadata*/,
                                             unsigned int /*soldier_id*/) override
    {
        return result_;
    }

    NetworkEventObserverResult OnPingCheck(
      const ConnectionMetadata& /*connection_metadata*/) override
    {
        return result_;
    }

    void SetResult(NetworkEventObserverResult result) { result_ = result; }

    unsigned int GetLastAssignedPlayerId() const { return last_assigned_player_id_; }
    std::string GetLastChatMessage() const { return last_chat_message_; }

private:
    NetworkEventObserverResult result_{ NetworkEventObserverResult::Success };
    unsigned int last_assigned_player_id_{};
    std::string last_chat_message_;
};

class NetworkEventDispatcherTests : public ::testing::Test
{
protected:
    NetworkEventDispatcherTests() = default;

    void SetUp() override
    {
        observer_.SetResult(NetworkEventObserverResult::Success);
        observer_ptr_ = std::shared_ptr<NetworkEventObserverExample>(&observer_, [](auto*) {});
        network_event_dispatcher_ = std::make_shared<NetworkEventDispatcher>(observer_ptr_);
    }

    void TearDown() override
    {
        observer_ptr_.reset();
        network_event_dispatcher_.reset();
    }

    NetworkEventDispatcher::TDispatchResult ProcessNetworkMessage(
      const NetworkMessage& network_message)
    {
        ConnectionMetadata connection_metadata;
        return network_event_dispatcher_->ProcessNetworkMessage(connection_metadata,
                                                                network_message);
    }

    NetworkEventObserverExample& GetObserver() { return observer_; }

private:
    NetworkEventObserverExample observer_;
    std::shared_ptr<INetworkEventObserver> observer_ptr_;
    std::shared_ptr<NetworkEventDispatcher> network_event_dispatcher_;
};

TEST_F(NetworkEventDispatcherTests, TestNetworkEventDispatcherParseError)
{
    std::vector<char> invalid_data{ 1 };
    NetworkMessage network_message(invalid_data);
    auto result = ProcessNetworkMessage(network_message);
    ASSERT_EQ(result.first, NetworkEventDispatchResult::ParseError);
    ASSERT_EQ(std::get<ParseError>(result.second), ParseError::BufferTooSmall);
}

TEST_F(NetworkEventDispatcherTests, TestNetworkEventDispatcherProcessAssignPlayerIdEvent)
{
    NetworkMessage network_message(NetworkEvent::AssignPlayerId, 5U);
    auto result = ProcessNetworkMessage(network_message);
    ASSERT_EQ(result.first, NetworkEventDispatchResult::Success);
    ASSERT_EQ(GetObserver().GetLastAssignedPlayerId(), 5U);
}

TEST_F(NetworkEventDispatcherTests, TestNetworkEventDispatcherParseErrorInAssignPlayerIdEvent)
{
    std::vector<char> invalid_data{ 1, 0, 0, 0, 1 };
    NetworkMessage network_message(invalid_data);
    auto result = ProcessNetworkMessage(network_message);
    ASSERT_EQ(result.first, NetworkEventDispatchResult::ParseError);
    ASSERT_EQ(std::get<ParseError>(result.second), ParseError::BufferTooSmall);
}

TEST_F(NetworkEventDispatcherTests, TestNetworkEventDispatcherAssignPlayerIdEventFailure)
{
    GetObserver().SetResult(NetworkEventObserverResult::Failure);
    NetworkMessage network_message(NetworkEvent::AssignPlayerId, 5U);
    auto result = ProcessNetworkMessage(network_message);
    ASSERT_EQ(result.first, NetworkEventDispatchResult::ObserverFailure);
    ASSERT_EQ(std::get<NetworkEventObserverResult>(result.second),
              NetworkEventObserverResult::Failure);
}

TEST_F(NetworkEventDispatcherTests, TestNetworkEventDispatcherProcessChatMessageEvent)
{
    NetworkMessage network_message(NetworkEvent::ChatMessage, "Test Message");
    auto result = ProcessNetworkMessage(network_message);
    ASSERT_EQ(result.first, NetworkEventDispatchResult::Success);
    ASSERT_EQ(GetObserver().GetLastChatMessage(), "Test Message");
}

TEST_F(NetworkEventDispatcherTests, TestNetworkEventDispatcherParseErrorInChatMessageEvent)
{
    std::vector<char> invalid_data{ 0, 0, 0, 0, 5, 0, 'T', 'e', 's', 't' };
    NetworkMessage network_message(invalid_data);
    auto result = ProcessNetworkMessage(network_message);
    ASSERT_EQ(result.first, NetworkEventDispatchResult::ParseError);
    ASSERT_EQ(std::get<ParseError>(result.second), ParseError::InvalidStringSize);
}

TEST_F(NetworkEventDispatcherTests, TestNetworkEventDispatcherProcessChatMessageEventFailure)
{
    GetObserver().SetResult(NetworkEventObserverResult::Failure);
    NetworkMessage network_message(NetworkEvent::ChatMessage, "Test Message");
    auto result = ProcessNetworkMessage(network_message);
    ASSERT_EQ(result.first, NetworkEventDispatchResult::ObserverFailure);
    ASSERT_EQ(std::get<NetworkEventObserverResult>(result.second),
              NetworkEventObserverResult::Failure);
}

TEST_F(NetworkEventDispatcherTests, TestNetworkEventDispatcherParseErrorInvalidNetworkEvent)
{
    std::vector<char> invalid_data{ 100, 100, 0, 0 };
    NetworkMessage network_message(invalid_data);
    auto result = ProcessNetworkMessage(network_message);
    ASSERT_EQ(result.first, NetworkEventDispatchResult::ParseError);
    ASSERT_EQ(std::get<ParseError>(result.second), ParseError::InvalidNetworkEvent);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}