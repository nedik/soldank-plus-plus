#include "World.hpp"

#include "core/state/State.hpp"
#include "core/state/Control.hpp"
#include "core/physics/Soldier.hpp"

#include "application/input/Keyboard.hpp"
#include "application/input/Mouse.hpp"
#include "application/config/Config.hpp"

#include "rendering/Scene.hpp"
#include "rendering/data/Sprites.hpp"

#include <GLFW/glfw3.h>

#include <memory>

namespace Soldat::World
{
namespace
{
std::shared_ptr<State> state;
std::shared_ptr<Control> control;
std::unique_ptr<Scene> scene;
std::unique_ptr<Soldier> soldier;
} // namespace

void Init()
{
    Sprites::Init();

    state = std::make_shared<State>("maps/ctf_Ash.pms");
    control = std::make_shared<Control>();
    scene = std::make_unique<Scene>(state);
    soldier = std::make_unique<Soldier>(
      glm::vec2(state->map.GetSpawnPoints()[0].x, state->map.GetSpawnPoints()[0].y));
}

void UpdateControlFromUserInput()
{
    control->left = Keyboard::Key(GLFW_KEY_A);
    control->right = Keyboard::Key(GLFW_KEY_D);
    control->up = Keyboard::Key(GLFW_KEY_W);
    control->down = Keyboard::Key(GLFW_KEY_S);
    control->change = Keyboard::Key(GLFW_KEY_Q);
    control->throw_grenade = Keyboard::Key(GLFW_KEY_E);
    control->drop = Keyboard::Key(GLFW_KEY_F);
    control->prone = Keyboard::Key(GLFW_KEY_X);

    control->fire = Mouse::Button(GLFW_MOUSE_BUTTON_LEFT);
    control->jets = Mouse::Button(GLFW_MOUSE_BUTTON_RIGHT);

    soldier->UpdateKeys(*control);
}

void Update(double /*delta_time*/)
{
    if (Config::DEBUG_DRAW) {
        state->colliding_polygon_ids.clear();
    }
    UpdateControlFromUserInput();
    state->mouse.x = (float)Mouse::GetX();
    state->mouse.y = 480.0F - (float)Mouse::GetY(); // TODO: soldier.control.mouse_aim_y expects top
                                                    // to be 0 and bottom to be game_height
    state->game_width = 640.0;
    state->game_height = 480.0;
    state->camera.x = soldier->particle.position.x + (float)(Mouse::GetX() - 320.0);
    state->camera.y = soldier->particle.position.y - (float)(Mouse::GetY() - 240.0);
    soldier->Update(*state);
}

void Render(double frame_percent)
{
    state->mouse.x = (float)Mouse::GetX();
    state->mouse.y = 480.0F - (float)Mouse::GetY();
    scene->Render(state, *soldier, frame_percent);
}

void Free()
{
    scene.reset(nullptr);
    soldier.reset(nullptr);
    control.reset();
    state.reset();
}
} // namespace Soldat::World