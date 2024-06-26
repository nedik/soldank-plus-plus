#include "Scene.hpp"

#include "rendering/renderer/interface/debug/DebugUI.hpp"

#include "application/input/Mouse.hpp"
#include "application/config/Config.hpp"

#include "core/entities/Bullet.hpp"
#include "core/math/Calc.hpp"

#include <string>
#include <algorithm>
#include <vector>
#include <format>

namespace Soldank
{
Scene::Scene(const std::shared_ptr<StateManager>& game_state)
    : background_renderer_(game_state->GetState().map.GetBackgroundTopColor(),
                           game_state->GetState().map.GetBackgroundBottomColor(),
                           game_state->GetState().map.GetBoundaries())
    , polygons_renderer_(game_state->GetState().map.GetPolygons(),
                         std::string(game_state->GetState().map.GetTextureName()))
    , polygon_outlines_renderer_(game_state->GetState().map.GetPolygons())
    , sceneries_renderer_(game_state->GetState().map.GetSceneryTypes(),
                          game_state->GetState().map.GetSceneryInstances())
    , soldier_renderer_(sprite_manager_)
    , text_renderer_("play-regular.ttf", 48)
    , bullet_renderer_(sprite_manager_)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Scene::Render(State& game_state, ClientState& client_state, double frame_percent, int fps)
{
    glm::vec2 new_camera_position =
      Calc::Lerp(client_state.camera_prev, client_state.camera, (float)frame_percent);
    camera_.Move(new_camera_position.x, new_camera_position.y);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    background_renderer_.Render(camera_.GetView());
    sceneries_renderer_.Render(camera_.GetView(), 0, game_state.map.GetSceneryInstances());
    if (client_state.draw_server_pov_client_pos) {
        rectangle_renderer_.Render(camera_.GetView(), client_state.soldier_position_server_pov);
    }
    RenderSoldiers(game_state, client_state, frame_percent);
    for (const Bullet& bullet : game_state.bullets) {
        bullet_renderer_.Render(camera_.GetView(), bullet, frame_percent);
    }
    sceneries_renderer_.Render(camera_.GetView(), 1, game_state.map.GetSceneryInstances());
    polygons_renderer_.Render(camera_.GetView());
    if (Config::DEBUG_DRAW) {
        std::sort(game_state.colliding_polygon_ids.begin(), game_state.colliding_polygon_ids.end());
        game_state.colliding_polygon_ids.erase(std::unique(game_state.colliding_polygon_ids.begin(),
                                                           game_state.colliding_polygon_ids.end()),
                                               game_state.colliding_polygon_ids.end());
        for (unsigned int polygon_id : game_state.colliding_polygon_ids) {
            polygon_outlines_renderer_.Render(camera_.GetView(), polygon_id);
        }
    }
    sceneries_renderer_.Render(camera_.GetView(), 2, game_state.map.GetSceneryInstances());

    DebugUI::Render(game_state, client_state, frame_percent, fps);
    if (!DebugUI::GetWantCaptureMouse()) {
        cursor_renderer_.Render({ client_state.mouse.x, client_state.mouse.y });
    }

    for (const auto& soldier : game_state.soldiers) {
        if (client_state.client_soldier_id.has_value() &&
            *client_state.client_soldier_id == soldier.id) {
            text_renderer_.Render("Health: " + std::to_string((int)soldier.health),
                                  50.0,
                                  100.0,
                                  1.0,
                                  { 1.0, 1.0, 1.0 });
            text_renderer_.Render("Jets: " + std::to_string((int)soldier.jets_count),
                                  50.0,
                                  50.0,
                                  1.0,
                                  { 1.0, 1.0, 1.0 });
        }
    }

    if (client_state.client_soldier_id.has_value()) {
        for (const auto& soldier : game_state.soldiers) {
            if (*client_state.client_soldier_id == soldier.id) {
                if (soldier.dead_meat) {
                    text_renderer_.Render(
                      "Respawn timer: " +
                        std::format("{:.2f}", (float)soldier.ticks_to_respawn / 60.0F),
                      400.0,
                      100.0,
                      1.0,
                      { 1.0, 1.0, 1.0 });
                }
            }
        }
    }

    if (Config::DEBUG_DRAW) {
        for (const auto& soldier : game_state.soldiers) {
            rectangle_renderer_.Render(
              camera_.GetView(),
              glm::vec2(soldier.control.mouse_aim_x, soldier.control.mouse_aim_y));
        }
    }
}

void Scene::RenderSoldiers(const State& game_state,
                           const ClientState& client_state,
                           double frame_percent)
{
    for (const auto& soldier : game_state.soldiers) {
        if (client_state.client_soldier_id.has_value() &&
            *client_state.client_soldier_id == soldier.id) {

            // skip rendering player's soldier sprites now because we will render it later
            continue;
        }
        if (soldier.active) {
            soldier_renderer_.Render(camera_.GetView(), sprite_manager_, soldier, frame_percent);
        }
    }

    // Render player's soldier last because it's the most important for the player to see their
    // soldier on top of others
    if (client_state.client_soldier_id.has_value()) {
        unsigned int client_soldier_id = *client_state.client_soldier_id;
        for (const auto& soldier : game_state.soldiers) {
            if (soldier.id == client_soldier_id && soldier.active) {
                soldier_renderer_.Render(
                  camera_.GetView(), sprite_manager_, soldier, frame_percent);
            }
        }
    }
}
} // namespace Soldank
