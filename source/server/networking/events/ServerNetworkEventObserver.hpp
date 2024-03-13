#ifndef __SERVER_NETWORK_EVENT_OBSERVER_HPP__
#define __SERVER_NETWORK_EVENT_OBSERVER_HPP__

#include "communication/NetworkEventDispatcher.hpp"

#include "application/ServerState.hpp"

#include "core/IWorld.hpp"

namespace Soldat
{
class ServerNetworkEventObserver : public INetworkEventObserver
{
public:
    ServerNetworkEventObserver(const std::shared_ptr<IWorld>& world,
                               const std::shared_ptr<ServerState>& server_state);

    NetworkEventObserverResult OnAssignPlayerId(const ConnectionMetadata& connection_metadata,
                                                unsigned int assigned_player_id) override;
    NetworkEventObserverResult OnChatMessage(const ConnectionMetadata& connection_metadata,
                                             const std::string& chat_message) override;
    NetworkEventObserverResult OnSpawnSoldier(const ConnectionMetadata& connection_metadata,
                                              unsigned int soldier_id,
                                              glm::vec2 spawn_position) override;
    NetworkEventObserverResult OnSoldierInput(const ConnectionMetadata& connection_metadata,
                                              unsigned int input_sequence_id,
                                              unsigned int soldier_id,
                                              glm::vec2 soldier_position,
                                              glm::vec2 mouse_position,
                                              const Control& player_control) override;

    NetworkEventObserverResult OnSoldierState(const ConnectionMetadata& connection_metadata,
                                              unsigned int soldier_id,
                                              glm::vec2 soldier_position,
                                              glm::vec2 soldier_old_position,
                                              AnimationType body_animation_type,
                                              unsigned int body_animation_frame,
                                              int body_animation_speed,
                                              AnimationType legs_animation_type,
                                              unsigned int legs_animation_frame,
                                              int legs_animation_speed,
                                              glm::vec2 soldier_velocity,
                                              glm::vec2 soldier_force,
                                              bool on_ground,
                                              bool on_ground_for_law,
                                              bool on_ground_last_frame,
                                              bool on_ground_permanent,
                                              unsigned int last_processed_input_id) override;

    NetworkEventObserverResult OnSoldierInfo(const ConnectionMetadata& connection_metadata,
                                             unsigned int soldier_id) override;

    NetworkEventObserverResult OnPlayerLeave(const ConnectionMetadata& connection_metadata,
                                             unsigned int soldier_id) override;

private:
    std::shared_ptr<IWorld> world_;
    std::shared_ptr<ServerState> server_state_;
};
} // namespace Soldat

#endif
