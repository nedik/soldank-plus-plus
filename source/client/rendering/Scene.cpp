#include "Scene.hpp"

#include "application/input/Mouse.hpp"
#include "application/config/Config.hpp"

#include "core/entities/Bullet.hpp"
#include "core/math/Calc.hpp"

#include <string>
#include <algorithm>

namespace Soldat
{
Scene::Scene(const std::shared_ptr<State>& state)
    : background_renderer_(state->map.GetBackgroundTopColor(),
                           state->map.GetBackgroundBottomColor(),
                           state->map.GetBoundaries())
    , polygons_renderer_(state->map.GetPolygons(), std::string(state->map.GetTextureName()))
    , polygon_outlines_renderer_(state->map.GetPolygons())
    , sceneries_renderer_(state->map.GetSceneryTypes(), state->map.GetSceneryInstances())
    , soldier_renderer_(sprite_manager_)
    , text_renderer_("play-regular.ttf", 48)
    , bullet_renderer_(sprite_manager_)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Scene::Render(const std::shared_ptr<State>& state,
                   const Soldier& soldier,
                   double frame_percent,
                   int fps)
{
    glm::vec2 new_camera_position =
      Calc::Lerp(state->camera_prev, state->camera, (float)frame_percent);
    camera_.Move(new_camera_position.x, -new_camera_position.y);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    background_renderer_.Render(camera_.GetView());
    sceneries_renderer_.Render(camera_.GetView(), 0, state->map.GetSceneryInstances());
    soldier_renderer_.Render(camera_.GetView(), sprite_manager_, soldier, frame_percent);
    for (const Bullet& bullet : state->bullets) {
        bullet_renderer_.Render(camera_.GetView(), bullet, frame_percent);
    }
    sceneries_renderer_.Render(camera_.GetView(), 1, state->map.GetSceneryInstances());
    polygons_renderer_.Render(camera_.GetView());
    if (Config::DEBUG_DRAW) {
        std::sort(state->colliding_polygon_ids.begin(), state->colliding_polygon_ids.end());
        state->colliding_polygon_ids.erase(
          std::unique(state->colliding_polygon_ids.begin(), state->colliding_polygon_ids.end()),
          state->colliding_polygon_ids.end());
        for (unsigned int polygon_id : state->colliding_polygon_ids) {
            polygon_outlines_renderer_.Render(camera_.GetView(), polygon_id);
        }
    }
    sceneries_renderer_.Render(camera_.GetView(), 2, state->map.GetSceneryInstances());
    cursor_renderer_.Render({ state->mouse.x, state->game_height - state->mouse.y });
    text_renderer_.Render("FPS: " + std::to_string(fps), 50.0, 100.0, 1.0, { 1.0, 1.0, 1.0 });
    if (Config::DEBUG_DRAW) {
        rectangle_renderer_.Render(
          camera_.GetView(), glm::vec2(soldier.control.mouse_aim_x, soldier.control.mouse_aim_y));
    }
}
} // namespace Soldat
