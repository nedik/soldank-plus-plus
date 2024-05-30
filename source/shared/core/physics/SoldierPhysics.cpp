#include "SoldierPhysics.hpp"

#include "core/animations/AnimationData.hpp"
#include "core/animations/AnimationState.hpp"
#include "core/physics/SoldierPhysics.hpp"
#include "core/physics/SoldierSkeletonPhysics.hpp"

#include "core/math/Calc.hpp"
#include "core/entities/Bullet.hpp"
#include "core/types/TeamType.hpp"
#include "core/types/WeaponType.hpp"
#include "core/entities/WeaponParametersFactory.hpp"

#include "spdlog/spdlog.h"

#include <cmath>
#include <utility>
#include <vector>
#include <algorithm>

const float SLIDELIMIT = 0.2F;
const float GRAV = 0.06F;
const float SURFACECOEFX = 0.970F;
const float SURFACECOEFY = 0.970F;
const float CROUCHMOVESURFACECOEFX = 0.85F;
const float CROUCHMOVESURFACECOEFY = 0.97F;
const float STANDSURFACECOEFX = 0.00F;
const float STANDSURFACECOEFY = 0.00F;

const std::uint8_t STANCE_STAND = 1;
const std::uint8_t STANCE_CROUCH = 2;
const std::uint8_t STANCE_PRONE = 3;

const float MAX_VELOCITY = 11.0F;
const float SOLDIER_COL_RADIUS = 3.0F;

const float RUNSPEED = 0.118F;
const float RUNSPEEDUP = RUNSPEED / 6.0F;
const float FLYSPEED = 0.03F;
const float JUMPSPEED = 0.66F;
const float CROUCHRUNSPEED = RUNSPEED / 0.6F;
const float PRONESPEED = RUNSPEED * 4.0F;
const float ROLLSPEED = RUNSPEED / 1.2F;
const float JUMPDIRSPEED = 0.30F;
const float JETSPEED = 0.10F;
const int SECOND = 60;

const int DEFAULT_IDLETIME = SECOND * 8;

namespace Soldank::SoldierPhysics
{
const Weapon& GetPrimaryWeapon(Soldier& soldier)
{
    return soldier.weapons[soldier.active_weapon];
}

const Weapon& GetSecondaryWeapon(Soldier& soldier)
{
    return soldier.weapons[(soldier.active_weapon + 1) % 2];
}

const Weapon& GetTertiaryWeapon(Soldier& soldier)
{
    return soldier.weapons[2];
}

void SwitchWeapon(Soldier& soldier)
{
    int new_active_weapon = (soldier.active_weapon + 1) % 2;
    soldier.active_weapon = new_active_weapon;
    // weapons[new_active_weapon].start_up_time_count =
    //   weapons[new_active_weapon].GetWeaponParameters().start_up_time;
    soldier.weapons[new_active_weapon].ResetStartUpTimeCount();
    // weapons[new_active_weapon].reload_time_prev = weapons[new_active_weapon].reload_time_count;
    soldier.weapons[new_active_weapon].SetReloadTimePrev(
      soldier.weapons[new_active_weapon].GetReloadTimeCount());
}

void UpdateKeys(Soldier& soldier, const Control& control)
{
    soldier.control = control;
}

void HandleSpecialPolytypes(const Map& map, PMSPolygonType polytype, Soldier& soldier)
{
    if (polytype == PMSPolygonType::Deadly || polytype == PMSPolygonType::BloodyDeadly ||
        polytype == PMSPolygonType::Explosive) {
        soldier.particle.position = glm::vec2(map.GetSpawnPoints()[0].x, map.GetSpawnPoints()[0].y);
    }
}

void Update(State& state,
            Soldier& soldier,
            const PhysicsEvents& physics_events,
            const AnimationDataManager& animation_data_manager,
            std::vector<BulletParams>& bullet_emitter)
{
    const Map& map = state.map;
    float body_y = 0.0f;
    float arm_s;

    soldier.particle.Euler();

    if (!soldier.control.fire) {
        soldier.is_shooting = false;
    }

    if (soldier.legs_animation->GetSpeed() < 1) {
        soldier.legs_animation->SetSpeed(1);
    }

    if (soldier.legs_animation->GetSpeed() < 1) {
        soldier.legs_animation->SetSpeed(1);
    }

    if (soldier.body_animation->GetSpeed() < 1) {
        soldier.body_animation->SetSpeed(1);
    }

    soldier.control.mouse_aim_x =
      (soldier.mouse.x - (float)soldier.game_width / 2.0 + soldier.camera.x);
    soldier.control.mouse_aim_y =
      (soldier.mouse.y - (float)soldier.game_height / 2.0 + soldier.camera.y);

    auto conflicting_keys_pressed = [](const Control& c) {
        return ((int)c.throw_grenade + (int)c.change + (int)c.drop + (int)c.reload) > 1;
    };

    // Handle simultaneous key presses that would conflict
    if (conflicting_keys_pressed(soldier.control)) {
        // At least two buttons pressed, so deactivate any previous one
        if (soldier.control.was_throwing_grenade) {
            soldier.control.throw_grenade = false;
        } else if (soldier.control.was_changing_weapon) {
            soldier.control.change = false;
        } else if (soldier.control.was_throwing_weapon) {
            soldier.control.drop = false;
        } else if (soldier.control.was_reloading_weapon) {
            soldier.control.reload = false;
        }

        // If simultaneously pressing two or more new buttons, then deactivate them
        // in order of least preference
        while (conflicting_keys_pressed(soldier.control)) {
            if (soldier.control.reload) {
                soldier.control.reload = false;
            } else if (soldier.control.change) {
                soldier.control.change = false;
            } else if (soldier.control.drop) {
                soldier.control.drop = false;
            } else if (soldier.control.throw_grenade) {
                soldier.control.throw_grenade = false;
            }
        }
    } else {
        soldier.control.was_throwing_grenade = soldier.control.throw_grenade;
        soldier.control.was_changing_weapon = soldier.control.change;
        soldier.control.was_throwing_weapon = soldier.control.drop;
        soldier.control.was_reloading_weapon = soldier.control.reload;
    }

    // self.fired = 0;
    soldier.control.mouse_aim_x =
      (int)((float)soldier.control.mouse_aim_x + soldier.particle.GetVelocity().x);
    soldier.control.mouse_aim_y =
      (int)((float)soldier.control.mouse_aim_y + soldier.particle.GetVelocity().y);

    auto maybe_new_legs_animation_state_machine = soldier.legs_animation->HandleInput(soldier);
    if (maybe_new_legs_animation_state_machine.has_value()) {
        soldier.legs_animation->Exit(soldier, physics_events);
        soldier.legs_animation = *maybe_new_legs_animation_state_machine;
        soldier.legs_animation->Enter(soldier);
    }

    soldier.body_animation->TryToShoot(soldier, physics_events);
    auto maybe_new_body_animation_state_machine = soldier.body_animation->HandleInput(soldier);
    if (maybe_new_body_animation_state_machine.has_value()) {
        soldier.body_animation->Exit(soldier, physics_events);
        soldier.body_animation = *maybe_new_body_animation_state_machine;
        soldier.body_animation->Enter(soldier);
    }

    soldier.legs_animation->Update(soldier, physics_events);
    soldier.body_animation->Update(soldier, physics_events);

    RepositionSoldierSkeletonParts(soldier);

    for (int i = 1; i <= 20; i++) {
        if ((soldier.dead_meat || soldier.half_dead) && (i < 17) && (i != 7) && (i != 8)) {
            auto xy = soldier.particle.position;
            soldier.on_ground = CheckSkeletonMapCollision(soldier, map, i, xy.x, xy.y, state);
        }
    }

    if (!soldier.dead_meat) {
        soldier.body_animation->DoAnimation();
        soldier.legs_animation->DoAnimation();

        soldier.on_ground = false;

        auto xy = soldier.particle.position;

        CheckMapCollision(soldier, map, xy.x - 3.5, xy.y - 12.0, 1, state);

        xy = soldier.particle.position;

        CheckMapCollision(soldier, map, xy.x + 3.5, xy.y - 12.0, 1, state);

        body_y = 0.0;
        arm_s = 0.0;

        // Walking either left or right (though only one can be active at once)
        if (soldier.control.left ^ soldier.control.right) {
            if (soldier.control.left ^ (soldier.direction == 1)) {
                // WRONG
                arm_s = 0.25;
            } else {
                body_y = 0.25;
            }
        }
        // If a leg is inside a polygon, caused by the modification of ArmS and
        // BodyY, this is there to not lose contact to ground on slope polygons
        if (body_y == 0.0) {
            // let leg_vector = vec2(
            //   self.particle.pos.x + 2.0,
            //   self.particle.pos.y + 1.9,
            //);
            //     if Map.RayCast(LegVector, LegVector, LegDistance, 10) {
            body_y = 0.25;
            // }
        }
        if (arm_s == 0.0) {
            // let leg_vector = vec2(
            //   self.particle.pos.x - 2.0,
            //   self.particle.pos.y + 1.9,
            //);
            //     if Map.RayCast(LegVector, LegVector, LegDistance, 10) {
            arm_s = 0.25;
            // }
        }

        xy = soldier.particle.position;
        soldier.on_ground =
          CheckMapCollision(soldier, map, xy.x + 2.0, xy.y + 2.0 - body_y, 0, state);

        xy = soldier.particle.position;
        soldier.on_ground =
          soldier.on_ground ||
          CheckMapCollision(soldier, map, xy.x - 2.0, xy.y + 2.0 - arm_s, 0, state);

        xy = soldier.particle.position;
        auto grounded = soldier.on_ground;
        soldier.on_ground_for_law =
          CheckRadiusMapCollision(soldier, map, xy.x, xy.y - 1.0, grounded, state);

        xy = soldier.particle.position;
        grounded = soldier.on_ground || soldier.on_ground_for_law;
        soldier.on_ground = soldier.on_ground || CheckMapVerticesCollision(
                                                   soldier, map, xy.x, xy.y, 3.0, grounded, state);

        if (!(soldier.on_ground ^ soldier.on_ground_last_frame)) {
            soldier.on_ground_permanent = soldier.on_ground;
        }

        soldier.on_ground_last_frame = soldier.on_ground;

        if ((soldier.jets_count < map.GetJetCount()) && !(soldier.control.jets)) {
            // if self.on_ground
            /* (MainTickCounter mod 2 = 0) */
            {
                soldier.jets_count += 1;
            }
        }
        soldier.alpha = 255;

        soldier.skeleton->DoVerletTimestepFor(22, 29);
        soldier.skeleton->DoVerletTimestepFor(24, 30);
    }

    if (soldier.dead_meat) {
        soldier.skeleton->DoVerletTimestep();
        soldier.particle.position = soldier.skeleton->GetPos(12);
        // CheckSkeletonOutOfBounds;
    }

    if (soldier.particle.velocity_.x > MAX_VELOCITY) {
        soldier.particle.velocity_.x = MAX_VELOCITY;
    }
    if (soldier.particle.velocity_.x < -MAX_VELOCITY) {
        soldier.particle.velocity_.x = -MAX_VELOCITY;
    }
    if (soldier.particle.velocity_.y > MAX_VELOCITY) {
        soldier.particle.velocity_.y = MAX_VELOCITY;
    }
    if (soldier.particle.velocity_.y < -MAX_VELOCITY) {
        soldier.particle.velocity_.y = MAX_VELOCITY;
    }
}

bool CheckMapCollision(Soldier& soldier, const Map& map, float x, float y, int area, State& state)
{
    auto pos = glm::vec2(x, y) + soldier.particle.velocity_;
    auto rx = ((int)std::round((pos.x / (float)map.GetSectorsSize()))) + 25;
    auto ry = ((int)std::round((pos.y / (float)map.GetSectorsSize()))) + 25;

    if ((rx > 0) && (rx < map.GetSectorsCount() + 25) && (ry > 0) &&
        (ry < map.GetSectorsCount() + 25)) {
        for (int j = 0; j < map.GetSector(rx, ry).polygons.size(); j++) {
            auto poly = map.GetSector(rx, ry).polygons[j] - 1;
            auto polytype = map.GetPolygons()[poly].polygon_type;

            if ((polytype != PMSPolygonType::NoCollide) &&
                (polytype != PMSPolygonType::OnlyBulletsCollide)) {
                auto polygons = map.GetPolygons()[poly];

                if (map.PointInPoly(pos, polygons)) {
                    HandleSpecialPolytypes(map, polytype, soldier);

                    auto dist = 0.0f;
                    auto k = 0;

                    auto perp = map.ClosestPerpendicular(poly, pos, &dist, &k);

                    auto step = perp;

                    perp = Calc::Vec2Normalize(perp);
                    perp *= dist;
                    dist = Calc::Vec2Length(soldier.particle.velocity_);

                    if (Calc::Vec2Length(perp) > dist) {
                        perp = Calc::Vec2Normalize(perp);
                        perp *= dist;
                    }
                    if ((area == 0) ||
                        ((area == 1) && ((soldier.particle.velocity_.y < 0.0) ||
                                         (soldier.particle.velocity_.x > SLIDELIMIT) ||
                                         (soldier.particle.velocity_.x < -SLIDELIMIT)))) {
                        soldier.particle.old_position = soldier.particle.position;
                        soldier.particle.position -= perp;
                        if (map.GetPolygons()[poly].polygon_type == PMSPolygonType::Bouncy) {
                            perp = Calc::Vec2Normalize(perp);
                            perp *= map.GetPolygons()[poly].bounciness * dist;
                        }
                        soldier.particle.velocity_ -= perp;
                    }

                    if (area == 0) {
                        if ((soldier.legs_animation->GetType() == AnimationType::Stand) ||
                            (soldier.legs_animation->GetType() == AnimationType::Crouch) ||
                            (soldier.legs_animation->GetType() == AnimationType::Prone) ||
                            (soldier.legs_animation->GetType() == AnimationType::ProneMove) ||
                            (soldier.legs_animation->GetType() == AnimationType::GetUp) ||
                            (soldier.legs_animation->GetType() == AnimationType::Fall) ||
                            (soldier.legs_animation->GetType() == AnimationType::Mercy) ||
                            (soldier.legs_animation->GetType() == AnimationType::Mercy2) ||
                            (soldier.legs_animation->GetType() == AnimationType::Own)) {
                            if ((soldier.particle.velocity_.x < SLIDELIMIT) &&
                                (soldier.particle.velocity_.x > -SLIDELIMIT) &&
                                (step.y > SLIDELIMIT)) {
                                soldier.particle.position = soldier.particle.old_position;
                                glm::vec2 particle_force = soldier.particle.GetForce();
                                particle_force.y -= GRAV;
                                soldier.particle.SetForce(particle_force);
                            }

                            if ((step.y > SLIDELIMIT) && (polytype != PMSPolygonType::Ice) &&
                                (polytype != PMSPolygonType::Bouncy)) {
                                if ((soldier.legs_animation->GetType() == AnimationType::Stand) ||
                                    (soldier.legs_animation->GetType() == AnimationType::Fall) ||
                                    (soldier.legs_animation->GetType() == AnimationType::Crouch)) {
                                    soldier.particle.velocity_.x *= STANDSURFACECOEFX;
                                    soldier.particle.velocity_.y *= STANDSURFACECOEFY;

                                    glm::vec2 particle_force = soldier.particle.GetForce();
                                    particle_force.x -= soldier.particle.velocity_.x;
                                    soldier.particle.SetForce(particle_force);
                                } else if (soldier.legs_animation->GetType() ==
                                           AnimationType::Prone) {
                                    if (soldier.legs_animation->GetFrame() > 24) {
                                        if (!(soldier.control.down &&
                                              (soldier.control.left || soldier.control.right))) {
                                            soldier.particle.velocity_.x *= STANDSURFACECOEFX;
                                            soldier.particle.velocity_.y *= STANDSURFACECOEFY;

                                            glm::vec2 particle_force = soldier.particle.GetForce();
                                            particle_force.x -= soldier.particle.velocity_.x;
                                            soldier.particle.SetForce(particle_force);
                                        }
                                    } else {
                                        soldier.particle.velocity_.x *= SURFACECOEFX;
                                        soldier.particle.velocity_.y *= SURFACECOEFY;
                                    }
                                } else if (soldier.legs_animation->GetType() ==
                                           AnimationType::GetUp) {
                                    soldier.particle.velocity_.x *= SURFACECOEFX;
                                    soldier.particle.velocity_.y *= SURFACECOEFY;
                                } else if (soldier.legs_animation->GetType() ==
                                           AnimationType::ProneMove) {
                                    soldier.particle.velocity_.x *= STANDSURFACECOEFX;
                                    soldier.particle.velocity_.y *= STANDSURFACECOEFY;
                                }
                            }
                        } else if ((soldier.legs_animation->GetType() ==
                                    AnimationType::CrouchRun) ||
                                   (soldier.legs_animation->GetType() ==
                                    AnimationType::CrouchRunBack)) {
                            soldier.particle.velocity_.x *= CROUCHMOVESURFACECOEFX;
                            soldier.particle.velocity_.y *= CROUCHMOVESURFACECOEFY;
                        } else {
                            soldier.particle.velocity_.x *= SURFACECOEFX;
                            soldier.particle.velocity_.y *= SURFACECOEFY;
                        }
                    }

                    return true;
                }
            }
        }
    }

    return false;
}

bool CheckMapVerticesCollision(Soldier& soldier,
                               const Map& map,
                               float x,
                               float y,
                               float r,
                               bool has_collided,
                               State& state)
{
    auto pos = glm::vec2(x, y) + soldier.particle.velocity_;
    auto rx = ((int)std::round(pos.x / (float)map.GetSectorsSize())) + 25;
    auto ry = ((int)std::round(pos.y / (float)map.GetSectorsSize())) + 25;

    if ((rx > 0) && (rx < map.GetSectorsCount() + 25) && (ry > 0) &&
        (ry < map.GetSectorsCount() + 25)) {
        for (int j = 0; j < map.GetSector(rx, ry).polygons.size(); j++) {
            auto poly = map.GetSector(rx, ry).polygons[j] - 1;
            PMSPolygonType polytype = map.GetPolygons()[poly].polygon_type;

            if (polytype != PMSPolygonType::NoCollide &&
                polytype != PMSPolygonType::OnlyBulletsCollide) {
                for (int i = 0; i < 3; i++) {
                    auto vert = glm::vec2(map.GetPolygons()[poly].vertices[i].x,
                                          map.GetPolygons()[poly].vertices[i].y);

                    auto dist = Calc::Distance(vert, pos);
                    if (dist < r) {
                        if (!has_collided) {
                            HandleSpecialPolytypes(map, polytype, soldier);
                        }
                        auto dir = pos - vert;
                        dir = Calc::Vec2Normalize(dir);
                        soldier.particle.position += dir;

                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool CheckRadiusMapCollision(Soldier& soldier,
                             const Map& map,
                             float x,
                             float y,
                             bool has_collided,
                             State& state)
{
    auto s_pos = glm::vec2(x, y - 3.0f);

    auto det_acc = (int)Calc::Vec2Length(soldier.particle.velocity_);
    if (det_acc == 0) {
        det_acc = 1;
    }

    auto step = soldier.particle.velocity_ * (1.0f / (float)det_acc);

    for (int _z = 0; _z < det_acc; _z++) {
        s_pos += step;

        auto rx = ((int)std::round(s_pos.x / (float)map.GetSectorsSize())) + 25;
        auto ry = ((int)std::round(s_pos.y / (float)map.GetSectorsSize())) + 25;

        if ((rx > 0) && (rx < map.GetSectorsCount() + 25) && (ry > 0) &&
            (ry < map.GetSectorsCount() + 25)) {
            for (int j = 0; j < map.GetSector(rx, ry).polygons.size(); j++) {
                auto poly = map.GetSector(rx, ry).polygons[j] - 1;
                auto polytype = map.GetPolygons()[poly].polygon_type;

                if (polytype != PMSPolygonType::NoCollide &&
                    polytype != PMSPolygonType::OnlyBulletsCollide) {
                    for (int k = 0; k < 2; k++) { // TODO: czy tu powinno być k < 3?
                        auto norm = glm::vec2(map.GetPolygons()[poly].perpendiculars[k].x,
                                              map.GetPolygons()[poly].perpendiculars[k].y);
                        norm *= -SOLDIER_COL_RADIUS;

                        auto pos = s_pos + norm;

                        if (map.PointInPolyEdges(pos.x, pos.y, poly)) {
                            if (!has_collided) {
                                HandleSpecialPolytypes(map, polytype, soldier);
                            }
                            auto d = 0.0f;
                            auto b = 0;
                            auto perp = map.ClosestPerpendicular(poly, pos, &d, &b);

                            auto p1 = glm::vec2(0.0, 0.0);
                            auto p2 = glm::vec2(0.0, 0.0);
                            switch (b) {
                                case 1: {
                                    p1 = glm::vec2(map.GetPolygons()[poly].vertices[0].x,
                                                   map.GetPolygons()[poly].vertices[0].y);
                                    p2 = glm::vec2(map.GetPolygons()[poly].vertices[1].x,
                                                   map.GetPolygons()[poly].vertices[1].y);
                                    break;
                                }
                                case 2: {
                                    p1 = glm::vec2(map.GetPolygons()[poly].vertices[1].x,
                                                   map.GetPolygons()[poly].vertices[1].y);
                                    p2 = glm::vec2(map.GetPolygons()[poly].vertices[2].x,
                                                   map.GetPolygons()[poly].vertices[2].y);
                                    break;
                                }
                                case 3: {
                                    p1 = glm::vec2(map.GetPolygons()[poly].vertices[2].x,
                                                   map.GetPolygons()[poly].vertices[2].y);
                                    p2 = glm::vec2(map.GetPolygons()[poly].vertices[0].x,
                                                   map.GetPolygons()[poly].vertices[0].y);
                                    break;
                                }
                            }

                            auto p3 = pos;
                            d = Calc::PointLineDistance(p1, p2, p3);
                            perp *= d;

                            soldier.particle.position = soldier.particle.old_position;
                            soldier.particle.velocity_ = soldier.particle.GetForce() - perp;

                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool CheckSkeletonMapCollision(Soldier& soldier,
                               const Map& map,
                               unsigned int i,
                               float x,
                               float y,
                               State& state)
{
    auto result = false;
    auto pos = glm::vec2(x - 1.0f, y + 4.0f);
    auto rx = ((int)std::round(pos.x / (float)map.GetSectorsSize())) + 25;
    auto ry = ((int)std::round(pos.y / (float)map.GetSectorsSize())) + 25;

    if ((rx > 0) && (rx < map.GetSectorsCount() + 25) && (ry > 0) &&
        (ry < map.GetSectorsCount() + 25)) {
        for (int j = 0; j < map.GetSector(rx, ry).polygons.size(); j++) {
            auto poly = map.GetSector(rx, ry).polygons[j] - 1;

            if (map.PointInPolyEdges(pos.x, pos.y, poly)) {
                auto dist = 0.0f;
                auto b = 0;
                auto perp = map.ClosestPerpendicular(poly, pos, &dist, &b);
                perp = Calc::Vec2Normalize(perp);
                perp *= dist;

                soldier.skeleton->SetPos(i, soldier.skeleton->GetOldPos(i) - perp);

                result = true;
            }
        }
    }

    if (result) {
        auto pos = glm::vec2(x, y + 1.0);
        auto rx = ((int)std::round(pos.x / (float)map.GetSectorsSize())) + 25;
        auto ry = ((int)std::round(pos.y / (float)map.GetSectorsSize())) + 25;

        if ((rx > 0) && (rx < map.GetSectorsCount() + 25) && (ry > 0) &&
            (ry < map.GetSectorsCount() + 25)) {
            for (int j = 0; j < map.GetSector(rx, ry).polygons.size(); j++) {
                auto poly = map.GetSector(rx, ry).polygons[j] - 1;
                // if (Map.PolyType[poly] <> POLY_TYPE_DOESNT) and (Map.PolyType[poly] <>
                // POLY_TYPE_ONLY_BULLETS) then
                if (map.PointInPolyEdges(pos.x, pos.y, poly)) {
                    auto dist = 0.0F;
                    auto b = 0;
                    auto perp = map.ClosestPerpendicular(poly, pos, &dist, &b);
                    perp = Calc::Vec2Normalize(perp);
                    perp *= dist;

                    soldier.skeleton->SetPos(i, soldier.skeleton->GetOldPos(i) - perp);

                    result = true;
                }
            }
        }
    }

    return result;
}

void Fire(Soldier& soldier, std::vector<BulletParams>& bullet_emitter)
{
    auto weapon = GetPrimaryWeapon(soldier);

    glm::vec2 dir;
    if (weapon.GetWeaponParameters().bullet_style == BulletType::Blade ||
        soldier.body_animation->GetType() == AnimationType::Mercy ||
        soldier.body_animation->GetType() == AnimationType::Mercy2) {
        dir = Calc::Vec2Normalize(soldier.skeleton->GetPos(15) - soldier.skeleton->GetPos(16));
    } else {
        auto aim_x = (float)soldier.control.mouse_aim_x;
        auto aim_y = (float)soldier.control.mouse_aim_y;
        dir = Calc::Vec2Normalize(glm::vec2(aim_x, aim_y) - soldier.skeleton->GetPos(15));
    };

    auto pos = soldier.skeleton->GetPos(15) + dir * 4.0F - glm::vec2(0.0, 2.0);
    auto bullet_velocity = dir * weapon.GetWeaponParameters().speed;
    auto inherited_velocity =
      soldier.particle.velocity_ * weapon.GetWeaponParameters().inherited_velocity;

    BulletParams params{
        weapon.GetWeaponParameters().bullet_style,
        weapon.GetWeaponParameters().kind,
        pos,
        bullet_velocity + inherited_velocity,
        (std::int16_t)weapon.GetWeaponParameters().timeout,
        weapon.GetWeaponParameters().hit_multiply,
        TeamType::None,
        soldier.id,
    };

    switch (weapon.GetWeaponParameters().kind) {
        case WeaponType::DesertEagles: {
            if (!soldier.is_shooting) {
                bullet_emitter.push_back(params);

                auto signx = dir.x > 0.0F ? 1.0F : (dir.x < 0.0F ? -1.0F : 0.0F);
                auto signy = dir.x > 0.0F ? 1.0F : (dir.x < 0.0F ? -1.0F : 0.0F);

                params.position += glm::vec2(-signx * dir.y, signy * dir.x) * 3.0F;
                bullet_emitter.push_back(params);
            }
            break;
        }
        case WeaponType::Spas12:
        case WeaponType::Flamer:
        case WeaponType::NoWeapon:
        case WeaponType::Knife:
        case WeaponType::Chainsaw:
            break;
        case WeaponType::LAW: {
            if ((soldier.on_ground || soldier.on_ground_permanent || soldier.on_ground_for_law) &&
                (((soldier.legs_animation->GetType() == AnimationType::Crouch &&
                   soldier.legs_animation->GetFrame() > 13) ||
                  soldier.legs_animation->GetType() == AnimationType::CrouchRun ||
                  soldier.legs_animation->GetType() == AnimationType::CrouchRunBack) ||
                 (soldier.legs_animation->GetType() == AnimationType::Prone &&
                  soldier.legs_animation->GetFrame() > 23))) {
                bullet_emitter.push_back(params);
            }
            break;
        }
        default: {
            bullet_emitter.push_back(params);
        }
    };

    soldier.is_shooting = true;
}
} // namespace Soldank::SoldierPhysics
