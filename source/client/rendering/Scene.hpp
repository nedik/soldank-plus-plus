#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "rendering/components/Camera.hpp"
#include "rendering/data/sprites/SpritesManager.hpp"
#include "rendering/renderer/BackgroundRenderer.hpp"
#include "rendering/renderer/PolygonsRenderer.hpp"
#include "rendering/renderer/PolygonOutlinesRenderer.hpp"
#include "rendering/renderer/SceneriesRenderer.hpp"
#include "rendering/renderer/SoldierRenderer.hpp"
#include "rendering/renderer/interface/CursorRenderer.hpp"
#include "rendering/renderer/TextRenderer.hpp"
#include "rendering/renderer/RectangleRenderer.hpp"

#include "core/state/State.hpp"
#include "core/physics/Soldier.hpp"

#include <memory>

namespace Soldat
{
class Scene
{
public:
    Scene(const std::shared_ptr<State>& state);

    void Render(const std::shared_ptr<State>& state, const Soldier& soldier, double frame_percent);

private:
    Camera camera_;
    Sprites::SpriteManager sprite_manager_;
    BackgroundRenderer background_renderer_;
    PolygonsRenderer polygons_renderer_;
    PolygonOutlinesRenderer polygon_outlines_renderer_;
    SceneriesRenderer sceneries_renderer_;
    SoldierRenderer soldier_renderer_;
    CursorRenderer cursor_renderer_;
    TextRenderer text_renderer_;
    RectangleRenderer rectangle_renderer_;
};
} // namespace Soldat

#endif
