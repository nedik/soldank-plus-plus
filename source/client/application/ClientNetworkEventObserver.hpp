#ifndef __CLIENT_NETWORK_EVENT_OBSERVER_HPP__
#define __CLIENT_NETWORK_EVENT_OBSERVER_HPP__

#include "communication/NetworkEventDispatcher.hpp"

#include "core/IWorld.hpp"

#include "rendering/ClientState.hpp"

#include <memory>

namespace Soldat
{
class ClientNetworkEventObserver : public INetworkEventObserver
{
public:
    ClientNetworkEventObserver(const std::shared_ptr<IWorld>& world,
                               const std::shared_ptr<ClientState>& client_state);

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
                                              std::uint8_t stance,
                                              float mouse_position_x,
                                              float mouse_position_y,
                                              bool using_jets,
                                              std::int32_t jets_count,
                                              unsigned int active_weapon,
                                              unsigned int last_processed_input_id) override;

    NetworkEventObserverResult OnSoldierInfo(const ConnectionMetadata& connection_metadata,
                                             unsigned int soldier_id) override;

    NetworkEventObserverResult OnPlayerLeave(const ConnectionMetadata& connection_metadata,
                                             unsigned int soldier_id) override;

    NetworkEventObserverResult OnPingCheck(const ConnectionMetadata& connection_metadata) override;

    NetworkEventObserverResult OnProjectileSpawn(const ConnectionMetadata& connection_metadata,
                                                 unsigned int projectile_id,
                                                 BulletType style,
                                                 WeaponType weapon,
                                                 float position_x,
                                                 float position_y,
                                                 float velocity_x,
                                                 float velocity_y,
                                                 std::int16_t timeout,
                                                 float hit_multiply,
                                                 TeamType team) override;

private:
    std::shared_ptr<IWorld> world_;
    std::shared_ptr<ClientState> client_state_;
};
} // namespace Soldat

#endif
