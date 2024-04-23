#include "rendering/renderer/interface/debug/DebugUI.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace Soldank::DebugUI
{
void Render(State& game_state, ClientState& client_state, double /*frame_percent*/, int fps)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent((client_state.mouse.x / 640.0F) * 1280.0F,
                        (480.0F - client_state.mouse.y) / 480.0F * 1024.0F);
    io.MouseDrawCursor = io.WantCaptureMouse;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("Debug window");
        ImGui::Checkbox("Server reconciliation", &client_state.server_reconciliation);
        ImGui::Checkbox("Client side prediction", &client_state.client_side_prediction);
        ImGui::Checkbox("Objects interpolation", &client_state.objects_interpolation);
        ImGui::Checkbox("Smooth camera", &client_state.smooth_camera);
        ImGui::Text("Application average %.3f ms/frame (%d FPS)", 1000.0F / (float)fps, fps);
        ImGui::Text("Non-acknowledged inputs: %llu", client_state.pending_inputs.size());
        ImGui::Text("Ping: %hu", client_state.ping_timer.GetLastPingMeasure());
        ImGui::SliderInt("Fake lag (milliseconds)", &client_state.network_lag, 0, 500);
        ImGui::Text("Bullets in game: %zu", game_state.bullets.size());
        ImGui::Checkbox("Draw server POV client position",
                        &client_state.draw_server_pov_client_pos);
        if (ImGui::Button("/kill")) {
            client_state.kill_button_just_pressed = true;
        }
        if (client_state.client_soldier_id.has_value()) {
            for (const auto& soldier : game_state.soldiers) {
                if (*client_state.client_soldier_id == soldier.id) {
                    ImGui::Text("Player ID: %d", soldier.id);
                    ImGui::Text("Position: %.3f, %.3f",
                                soldier.particle.position.x,
                                soldier.particle.position.y);

                    auto rx = ((int)((soldier.particle.position.x /
                                      (float)game_state.map.GetSectorsSize()))) +
                              25;
                    auto ry = ((int)((soldier.particle.position.y /
                                      (float)game_state.map.GetSectorsSize()))) +
                              25;

                    ImGui::Text("Sector: %d, %d", rx, ry);
                    if (soldier.active && soldier.on_ground) {
                        ImGui::Text("SOLDIER IS ON GROUND");
                    }
                }
            }
        }
        ImGui::End();
    }

    {
        ImGui::Begin("Weapons");

        ImGui::SeparatorText("Primary weapon");
        static std::vector<std::pair<std::string, WeaponType>> primary_weapons = {
            { "Desert Eagles", WeaponType::DesertEagles },
            { "HK MP5", WeaponType::MP5 },
            { "Ak-74", WeaponType::Ak74 },
            { "Steyr AUG", WeaponType::SteyrAUG },
            { "Spas-12", WeaponType::Spas12 },
            { "Ruger 77", WeaponType::Ruger77 },
            { "M79", WeaponType::M79 },
            { "Barret M82A1", WeaponType::Barrett },
            { "FN Minimi", WeaponType::Minimi },
            { "XM214 Minigun", WeaponType::Minigun },
        };
        for (const auto& weapon_data : primary_weapons) {
            if (ImGui::Selectable(weapon_data.first.c_str(),
                                  client_state.primary_weapon_type_choice == weapon_data.second)) {
                client_state.primary_weapon_type_choice = weapon_data.second;
            }
        }

        ImGui::NewLine();
        ImGui::SeparatorText("Secondary weapon");
        static std::vector<std::pair<std::string, WeaponType>> secondary_weapons = {
            { "USSOCOM", WeaponType::USSOCOM },
            { "Combat Knife", WeaponType::Knife },
            { "Chainsaw", WeaponType::Chainsaw },
            { "M72 LAW", WeaponType::LAW }
        };
        for (const auto& weapon_data : secondary_weapons) {
            if (ImGui::Selectable(weapon_data.first.c_str(),
                                  client_state.secondary_weapon_type_choice ==
                                    weapon_data.second)) {
                client_state.secondary_weapon_type_choice = weapon_data.second;
            }
        }
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool GetWantCaptureMouse()
{
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureMouse;
}
} // namespace Soldank::DebugUI
