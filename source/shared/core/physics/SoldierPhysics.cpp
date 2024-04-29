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

void LegsApplyAnimation(Soldier& soldier,
                        const AnimationDataManager& animation_data_manager,
                        AnimationType id,
                        unsigned int frame)
{
    if (!soldier.legs_animation.IsAny({ AnimationType::Prone, AnimationType::ProneMove }) &&
        soldier.legs_animation.GetType() != id) {
        soldier.legs_animation = AnimationState(animation_data_manager.Get(id));
        soldier.legs_animation.SetFrame(frame);
    }
}

void BodyApplyAnimation(Soldier& soldier,
                        const AnimationDataManager& animation_data_manager,
                        AnimationType id,
                        unsigned int frame)
{
    if (soldier.body_animation.GetType() != id) {
        soldier.body_animation = AnimationState(animation_data_manager.Get(id));
        soldier.body_animation.SetFrame(frame);
    }
}

void HandleSpecialPolytypes(const Map& map, PMSPolygonType polytype, Soldier& soldier)
{
    if (polytype == PMSPolygonType::Deadly || polytype == PMSPolygonType::BloodyDeadly ||
        polytype == PMSPolygonType::Explosive) {
        soldier.particle.position = glm::vec2(map.GetSpawnPoints()[0].x, map.GetSpawnPoints()[0].y);
    }
}

void UpdateControl(State& state,
                   Soldier& soldier,
                   const AnimationDataManager& animation_data_manager,
                   std::vector<BulletParams>& bullet_emitter)
{
    if (!soldier.control.fire) {
        soldier.is_shooting = false;
    }

    bool player_pressed_left_right = false;

    if (soldier.legs_animation.GetSpeed() < 1) {
        soldier.legs_animation.SetSpeed(1);
    }

    if (soldier.body_animation.GetSpeed() < 1) {
        soldier.body_animation.SetSpeed(1);
    }

    soldier.control.mouse_aim_x =
      (soldier.mouse.x - (float)soldier.game_width / 2.0 + soldier.camera.x);
    soldier.control.mouse_aim_y =
      (soldier.mouse.y - (float)soldier.game_height / 2.0 + soldier.camera.y);

    auto cleft = soldier.control.left;
    auto cright = soldier.control.right;

    // If both left and right directions are pressed, then decide which direction
    // to go in
    if (cleft && cright) {
        // Remember that both directions were pressed, as it's useful for some moves
        player_pressed_left_right = true;

        if (soldier.control.was_jumping) {
            // If jumping, keep going in the old direction
            if (soldier.control.was_running_left) {
                cright = false;
            } else {
                cleft = false;
            }
        } else {
            // If not jumping, instead go in the new direction
            if (soldier.control.was_running_left) {
                cleft = false;
            } else {
                cright = false;
            }
        }
    } else {
        soldier.control.was_running_left = cleft;
        soldier.control.was_jumping = soldier.control.up;
    }

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

    if (soldier.dead_meat) {
        // TODO: co to free controls?
        // control.free_controls();
    }

    // self.fired = 0;
    soldier.control.mouse_aim_x =
      (int)((float)soldier.control.mouse_aim_x + soldier.particle.GetVelocity().x);
    soldier.control.mouse_aim_y =
      (int)((float)soldier.control.mouse_aim_y + soldier.particle.GetVelocity().y);

    if (soldier.control.jets &&
        (((soldier.legs_animation.GetType() == AnimationType::JumpSide) &&
          (((soldier.direction == -1) && cright) || ((soldier.direction == 1) && cleft) ||
           player_pressed_left_right)) ||
         ((soldier.legs_animation.GetType() == AnimationType::RollBack) && soldier.control.up))) {
        BodyApplyAnimation(soldier, animation_data_manager, AnimationType::RollBack, 1);
        LegsApplyAnimation(soldier, animation_data_manager, AnimationType::RollBack, 1);
    } else if (soldier.control.jets && (soldier.jets_count > 0)) {
        if (soldier.on_ground) {
            // particle.force.y = -2.5 * (state.gravity > 0.05 ? JETSPEED :
            // state.gravity * 2.0);
            glm::vec2 particle_force = soldier.particle.GetForce();
            if (state.gravity > 0.05) {
                soldier.particle.SetForce({ particle_force.x, -2.5F * JETSPEED });
            } else {
                soldier.particle.SetForce({ particle_force.x, -2.5 * (state.gravity * 2.0F) });
            }
        } else if (soldier.stance != STANCE_PRONE) {
            // particle.force.y -= (state.gravity > 0.05 ? JETSPEED : state.gravity
            // * 2.0);
            glm::vec2 particle_force = soldier.particle.GetForce();
            if (state.gravity > 0.05) {
                soldier.particle.SetForce({ particle_force.x, particle_force.y - JETSPEED });
            } else {
                soldier.particle.SetForce(
                  { particle_force.x, particle_force.y - state.gravity * 2.0 });
            }
        } else {
            glm::vec2 particle_force = soldier.particle.GetForce();
            soldier.particle.SetForce(
              { particle_force.x + (float)soldier.direction *
                                     (state.gravity > 0.05 ? JETSPEED / 2.0 : state.gravity),
                particle_force.y });
        }

        if ((soldier.legs_animation.GetType() != AnimationType::GetUp) &&
            (soldier.body_animation.GetType() != AnimationType::Roll) &&
            (soldier.body_animation.GetType() != AnimationType::RollBack)) {
            LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Fall, 1);
        }
        soldier.jets_count -= 1;
    }

    // FIRE!!!!
    if (GetPrimaryWeapon(soldier).GetWeaponParameters().kind == WeaponType::Chainsaw ||
        (soldier.body_animation.GetType() != AnimationType::Roll) &&
          (soldier.body_animation.GetType() != AnimationType::RollBack) &&
          (soldier.body_animation.GetType() != AnimationType::Melee) &&
          (soldier.body_animation.GetType() != AnimationType::Change)) {
        if (((soldier.body_animation.GetType() == AnimationType::HandsUpAim) &&
             (soldier.body_animation.GetFrame() == 11)) ||
            (soldier.body_animation.GetType() != AnimationType::HandsUpAim)) {
            if (soldier.control.fire)
            // and (SpriteC.CeaseFireCounter < 0)
            {
                if (GetPrimaryWeapon(soldier).GetWeaponParameters().kind == WeaponType::NoWeapon ||
                    GetPrimaryWeapon(soldier).GetWeaponParameters().kind == WeaponType::Knife) {
                    BodyApplyAnimation(soldier, animation_data_manager, AnimationType::Punch, 1);
                } else {
                    Fire(soldier, bullet_emitter);
                }
            }
        }
    }

    ControlThrowGrenade(soldier, animation_data_manager);

    // change weapon animation
    if ((soldier.body_animation.GetType() != AnimationType::Roll) &&
        (soldier.body_animation.GetType() != AnimationType::RollBack)) {
        if (soldier.control.change) {
            BodyApplyAnimation(soldier, animation_data_manager, AnimationType::Change, 1);
        }
    }

    // change weapon
    if (soldier.body_animation.GetType() == AnimationType::Change) {
        if (soldier.body_animation.GetFrame() == 2) {
            // TODO: play sound
            soldier.body_animation.SetNextFrame();
        } else if (soldier.body_animation.GetFrame() == 25) {
            SwitchWeapon(soldier);
        } else if ((soldier.body_animation.GetFrame() ==
                    animation_data_manager.Get(AnimationType::Change)->GetFrames().size()) &&
                   (GetPrimaryWeapon(soldier).GetAmmoCount() == 0)) {
            BodyApplyAnimation(soldier, animation_data_manager, AnimationType::Stand, 1);
        }
    }

    // throw weapon
    if (soldier.control.drop &&
        (soldier.body_animation.GetType() != AnimationType::Change ||
         soldier.body_animation.GetFrame() > 25) &&
        !soldier.body_animation.IsAny(
          { AnimationType::Roll, AnimationType::RollBack, AnimationType::ThrowWeapon })
        // && !flamegod bonus
        && !GetPrimaryWeapon(soldier).IsAny({
             WeaponType::Bow,
             WeaponType::FlameBow,
             WeaponType::NoWeapon,
           })) {
        BodyApplyAnimation(soldier, animation_data_manager, AnimationType::ThrowWeapon, 1);

        if (GetPrimaryWeapon(soldier).GetWeaponParameters().kind == WeaponType::Knife) {
            soldier.body_animation.SetSpeed(2);
        }
    }

    // throw knife
    if (soldier.body_animation.GetType() == AnimationType::ThrowWeapon &&
        GetPrimaryWeapon(soldier).GetWeaponParameters().kind == WeaponType::Knife &&
        (!soldier.control.drop || soldier.body_animation.GetFrame() == 16)) {

        Weapon weapon{ WeaponParametersFactory::GetParameters(WeaponType::ThrownKnife, false) };
        auto aim_x = (float)soldier.control.mouse_aim_x;
        auto aim_y = (float)soldier.control.mouse_aim_y;
        auto dir = Calc::Vec2Normalize(glm::vec2(aim_x, aim_y) - soldier.skeleton->GetPos(15));
        auto frame = (float)soldier.body_animation.GetFrame();
        auto thrown_mul = 1.5F * std::min(16.0F, std::max(8.0F, frame)) / 16.0F;
        auto bullet_vel = dir * weapon.GetWeaponParameters().speed * thrown_mul;
        auto inherited_vel =
          soldier.particle.GetVelocity() * weapon.GetWeaponParameters().inherited_velocity;
        auto velocity = bullet_vel + inherited_vel;

        BulletParams params{
            weapon.GetWeaponParameters().bullet_style,
            weapon.GetWeaponParameters().kind,
            soldier.skeleton->GetPos(16) + velocity,
            velocity,
            (std::int16_t)weapon.GetWeaponParameters().timeout,
            weapon.GetWeaponParameters().hit_multiply,
            TeamType::None,
        };
        bullet_emitter.push_back(params);

        soldier.control.drop = false;
        BodyApplyAnimation(soldier, animation_data_manager, AnimationType::Stand, 1);
    }

    // Punch!
    if (!soldier.dead_meat) {
        if ((soldier.body_animation.GetType() == AnimationType::Punch) &&
            (soldier.body_animation.GetFrame() == 11)) {
            soldier.body_animation.SetNextFrame();
        }
    }

    // Buttstock!
    if (soldier.dead_meat) {
        if ((soldier.body_animation.GetType() == AnimationType::Melee) &&
            (soldier.body_animation.GetFrame() == 12)) {
            // weapons
        }
    }

    if (soldier.body_animation.GetType() == AnimationType::Melee &&
        soldier.body_animation.GetFrame() > 20) {
        BodyApplyAnimation(soldier, animation_data_manager, AnimationType::Stand, 1);
    }

    // Prone
    if (soldier.control.prone) {
        if ((soldier.legs_animation.GetType() != AnimationType::GetUp) &&
            (soldier.legs_animation.GetType() != AnimationType::Prone) &&
            (soldier.legs_animation.GetType() != AnimationType::ProneMove)) {
            LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Prone, 1);
            if ((soldier.body_animation.GetType() != AnimationType::Reload) &&
                (soldier.body_animation.GetType() != AnimationType::Change) &&
                (soldier.body_animation.GetType() != AnimationType::ThrowWeapon)) {
                BodyApplyAnimation(soldier, animation_data_manager, AnimationType::Prone, 1);
            }
            soldier.old_direction = soldier.direction;
            soldier.control.prone = false;
        }
    }

    // Get up
    if (soldier.stance == STANCE_PRONE) {
        if (soldier.control.prone || (soldier.direction != soldier.old_direction)) {
            if (((soldier.legs_animation.GetType() == AnimationType::Prone) &&
                 (soldier.legs_animation.GetFrame() > 23)) ||
                (soldier.legs_animation.GetType() == AnimationType::ProneMove)) {
                if (soldier.legs_animation.GetType() != AnimationType::GetUp) {
                    soldier.legs_animation =
                      AnimationState(animation_data_manager.Get(AnimationType::GetUp));
                    soldier.legs_animation.SetFrame(9);
                    soldier.control.prone = false;
                }
                if ((soldier.body_animation.GetType() != AnimationType::Reload) &&
                    (soldier.body_animation.GetType() != AnimationType::Change) &&
                    (soldier.body_animation.GetType() != AnimationType::ThrowWeapon)) {
                    BodyApplyAnimation(soldier, animation_data_manager, AnimationType::GetUp, 9);
                }
            }
        }
    }

    bool unprone = false;
    // Immediately switch from unprone to jump/sidejump, because the end of the
    // unprone animation can be seen as the "wind up" for the jump
    if ((soldier.legs_animation.GetType() == AnimationType::GetUp) &&
        (soldier.legs_animation.GetFrame() > 23 - (4 - 1)) && soldier.on_ground &&
        soldier.control.up && (cright || cleft)) {
        // Set sidejump frame 1 to 4 depending on which unprone frame we're in
        auto id = soldier.legs_animation.GetFrame() - (23 - (4 - 1));
        LegsApplyAnimation(soldier, animation_data_manager, AnimationType::JumpSide, id);
        unprone = true;
    } else if ((soldier.legs_animation.GetType() == AnimationType::GetUp) &&
               (soldier.legs_animation.GetFrame() > 23 - (4 - 1)) && soldier.on_ground &&
               soldier.control.up && !(cright || cleft)) {
        // Set jump frame 6 to 9 depending on which unprone frame we're in
        auto id = soldier.legs_animation.GetFrame() - (23 - (9 - 1));
        LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Jump, id);
        unprone = true;
    } else if ((soldier.legs_animation.GetType() == AnimationType::GetUp) &&
               (soldier.legs_animation.GetFrame() > 23)) {
        if (cright || cleft) {
            if ((soldier.direction == 1) ^ cleft) {
                LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Run, 1);
            } else {
                LegsApplyAnimation(soldier, animation_data_manager, AnimationType::RunBack, 1);
            }
        } else if (!soldier.on_ground && soldier.control.up) {
            LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Run, 1);
        } else {
            LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Stand, 1);
        }
        unprone = true;
    }

    if (unprone) {
        soldier.stance = STANCE_STAND;

        if ((soldier.body_animation.GetType() != AnimationType::Reload) &&
            (soldier.body_animation.GetType() != AnimationType::Change) &&
            (soldier.body_animation.GetType() != AnimationType::ThrowWeapon)) {
            BodyApplyAnimation(soldier, animation_data_manager, AnimationType::Stand, 1);
        }
    }

    if (true) {
        // self.stat == 0 {
        if (((soldier.body_animation.GetType() == AnimationType::Stand) &&
             (soldier.legs_animation.GetType() == AnimationType::Stand) && !soldier.dead_meat &&
             (soldier.idle_time > 0)) ||
            (soldier.idle_time > DEFAULT_IDLETIME)) {
            if (soldier.idle_random >= 0) {
                soldier.idle_time -= 1;
            }
        } else {
            soldier.idle_time = DEFAULT_IDLETIME;
        }

        if (soldier.idle_random == 0) {
            if (soldier.idle_time == 0) {
                BodyApplyAnimation(soldier, animation_data_manager, AnimationType::Smoke, 1);
                soldier.idle_time = DEFAULT_IDLETIME;
            }

            if ((soldier.body_animation.GetType() == AnimationType::Smoke) &&
                (soldier.body_animation.GetFrame() == 17)) {
                soldier.body_animation.SetNextFrame();
            }

            if (!soldier.dead_meat) {
                if ((soldier.idle_time == 1) &&
                    (soldier.body_animation.GetType() != AnimationType::Smoke) &&
                    (soldier.legs_animation.GetType() == AnimationType::Stand)) {
                    soldier.idle_time = DEFAULT_IDLETIME;
                    soldier.idle_random = -1;
                }
            }
        }

        // *CHEAT*
        if (soldier.legs_animation.GetSpeed() > 1) {
            if ((soldier.legs_animation.GetType() == AnimationType::Jump) ||
                (soldier.legs_animation.GetType() == AnimationType::JumpSide) ||
                (soldier.legs_animation.GetType() == AnimationType::Roll) ||
                (soldier.legs_animation.GetType() == AnimationType::RollBack) ||
                (soldier.legs_animation.GetType() == AnimationType::Prone) ||
                (soldier.legs_animation.GetType() == AnimationType::Run) ||
                (soldier.legs_animation.GetType() == AnimationType::RunBack)) {
                soldier.particle.velocity_.x /= (float)soldier.legs_animation.GetSpeed();
                soldier.particle.velocity_.y /= (float)soldier.legs_animation.GetSpeed();
            }

            if (soldier.legs_animation.GetSpeed() > 2) {
                if ((soldier.legs_animation.GetType() == AnimationType::ProneMove) ||
                    (soldier.legs_animation.GetType() == AnimationType::CrouchRun)) {
                    soldier.particle.velocity_.x /= (float)soldier.legs_animation.GetSpeed();
                    soldier.particle.velocity_.y /= (float)soldier.legs_animation.GetSpeed();
                }
            }
        }

        // TODO: Check if near collider

        // TODO if targetmode > freecontrols
        // End any ongoing idle animations if a key is pressed
        if ((soldier.body_animation.GetType() == AnimationType::Cigar) ||
            (soldier.body_animation.GetType() == AnimationType::Match) ||
            (soldier.body_animation.GetType() == AnimationType::Smoke) ||
            (soldier.body_animation.GetType() == AnimationType::Wipe) ||
            (soldier.body_animation.GetType() == AnimationType::Groin)) {
            if (cleft || cright || soldier.control.up || soldier.control.down ||
                soldier.control.fire || soldier.control.jets || soldier.control.drop ||
                soldier.control.change || soldier.control.change // TODO: 2x control.change
                || soldier.control.throw_grenade || soldier.control.reload ||
                soldier.control.prone) {
                soldier.body_animation.SetFrame(
                  animation_data_manager.Get(soldier.body_animation.GetType())
                    ->GetFrames()
                    .size()); // TODO: Check this if correct
            }
        }

        // make anims out of controls
        // rolling
        if ((soldier.body_animation.GetType() != AnimationType::TakeOff) &&
            (soldier.body_animation.GetType() != AnimationType::Piss) &&
            (soldier.body_animation.GetType() != AnimationType::Mercy) &&
            (soldier.body_animation.GetType() != AnimationType::Mercy2) &&
            (soldier.body_animation.GetType() != AnimationType::Victory) &&
            (soldier.body_animation.GetType() != AnimationType::Own)) {
            if ((soldier.body_animation.GetType() == AnimationType::Roll) ||
                (soldier.body_animation.GetType() == AnimationType::RollBack)) {
                if (soldier.legs_animation.GetType() == AnimationType::Roll) {
                    if (soldier.on_ground) {
                        glm::vec2 particle_force = soldier.particle.GetForce();
                        soldier.particle.SetForce(
                          { (float)soldier.direction * ROLLSPEED, particle_force.y });
                    } else {
                        glm::vec2 particle_force = soldier.particle.GetForce();
                        soldier.particle.SetForce(
                          { (float)soldier.direction * 2.0F * FLYSPEED, particle_force.y });
                    }
                } else if (soldier.legs_animation.GetType() == AnimationType::RollBack) {
                    if (soldier.on_ground) {
                        glm::vec2 particle_force = soldier.particle.GetForce();
                        particle_force.x = (-(float)soldier.direction) * ROLLSPEED;
                        soldier.particle.SetForce(particle_force);
                    } else {
                        glm::vec2 particle_force = soldier.particle.GetForce();
                        particle_force.x = (-(float)(soldier.direction)) * 2.0F * FLYSPEED;
                        soldier.particle.SetForce(particle_force);
                    }
                    // if appropriate frames to move
                    if ((soldier.legs_animation.GetFrame() > 1) &&
                        (soldier.legs_animation.GetFrame() < 8)) {
                        if (soldier.control.up) {
                            glm::vec2 particle_force = soldier.particle.GetForce();
                            particle_force.y -= JUMPDIRSPEED * 1.5F;
                            particle_force.x *= 0.5;
                            soldier.particle.SetForce(particle_force);
                            soldier.particle.velocity_.x *= 0.8;
                        }
                    }
                }
                // downright
            } else if ((cright) && (soldier.control.down)) {
                if (soldier.on_ground) {
                    // roll to the side
                    if ((soldier.legs_animation.GetType() == AnimationType::Run) ||
                        (soldier.legs_animation.GetType() == AnimationType::RunBack) ||
                        (soldier.legs_animation.GetType() == AnimationType::Fall) ||
                        (soldier.legs_animation.GetType() == AnimationType::ProneMove) ||
                        ((soldier.legs_animation.GetType() == AnimationType::Prone) &&
                         (soldier.legs_animation.GetFrame() >= 24))) {
                        if ((soldier.legs_animation.GetType() == AnimationType::ProneMove) ||
                            ((soldier.legs_animation.GetType() == AnimationType::Prone) &&
                             (soldier.legs_animation.GetFrame() ==
                              soldier.legs_animation.GetFramesCount()))) {
                            soldier.control.prone = false;
                            soldier.stance = STANCE_STAND;
                        }

                        if (soldier.direction == 1) {
                            BodyApplyAnimation(
                              soldier, animation_data_manager, AnimationType::Roll, 1);
                            soldier.legs_animation =
                              AnimationState(animation_data_manager.Get(AnimationType::Roll));
                            soldier.legs_animation.SetFrame(1);
                        } else {
                            BodyApplyAnimation(
                              soldier, animation_data_manager, AnimationType::RollBack, 1);
                            soldier.legs_animation =
                              AnimationState(animation_data_manager.Get(AnimationType::RollBack));
                            soldier.legs_animation.SetFrame(1);
                        }
                    } else {
                        if (soldier.direction == 1) {
                            LegsApplyAnimation(
                              soldier, animation_data_manager, AnimationType::CrouchRun, 1);
                        } else {
                            LegsApplyAnimation(
                              soldier, animation_data_manager, AnimationType::CrouchRunBack, 1);
                        }
                    }

                    if ((soldier.legs_animation.GetType() == AnimationType::CrouchRun) ||
                        (soldier.legs_animation.GetType() == AnimationType::CrouchRunBack)) {
                        glm::vec2 particle_force = soldier.particle.GetForce();
                        soldier.particle.SetForce({ CROUCHRUNSPEED, particle_force.y });
                    } else if ((soldier.legs_animation.GetType() == AnimationType::Roll) ||
                               (soldier.legs_animation.GetType() == AnimationType::RollBack)) {
                        glm::vec2 particle_force = soldier.particle.GetForce();
                        soldier.particle.SetForce({ 2.0F * CROUCHRUNSPEED, particle_force.y });
                    }
                }
                // downleft
            } else if (cleft && soldier.control.down) {
                if (soldier.on_ground) {
                    // roll to the side
                    if ((soldier.legs_animation.GetType() == AnimationType::Run) ||
                        (soldier.legs_animation.GetType() == AnimationType::RunBack) ||
                        (soldier.legs_animation.GetType() == AnimationType::Fall) ||
                        (soldier.legs_animation.GetType() == AnimationType::ProneMove) ||
                        ((soldier.legs_animation.GetType() == AnimationType::Prone) &&
                         (soldier.legs_animation.GetFrame() >= 24))) {
                        if ((soldier.legs_animation.GetType() == AnimationType::ProneMove) ||
                            ((soldier.legs_animation.GetType() == AnimationType::Prone) &&
                             (soldier.legs_animation.GetFrame() ==
                              soldier.legs_animation.GetFramesCount()))) {
                            soldier.control.prone = false;
                            soldier.stance = STANCE_STAND;
                        }

                        if (soldier.direction == 1) {
                            BodyApplyAnimation(
                              soldier, animation_data_manager, AnimationType::RollBack, 1);
                            soldier.legs_animation =
                              AnimationState(animation_data_manager.Get(AnimationType::RollBack));
                            soldier.legs_animation.SetFrame(1);
                        } else {
                            BodyApplyAnimation(
                              soldier, animation_data_manager, AnimationType::Roll, 1);
                            soldier.legs_animation =
                              AnimationState(animation_data_manager.Get(AnimationType::Roll));
                            soldier.legs_animation.SetFrame(1);
                        }
                    } else {
                        if (soldier.direction == 1) {
                            LegsApplyAnimation(
                              soldier, animation_data_manager, AnimationType::CrouchRunBack, 1);
                        } else {
                            LegsApplyAnimation(
                              soldier, animation_data_manager, AnimationType::CrouchRun, 1);
                        }
                    }

                    if ((soldier.legs_animation.GetType() == AnimationType::CrouchRun) ||
                        (soldier.legs_animation.GetType() == AnimationType::CrouchRunBack)) {
                        glm::vec2 particle_force = soldier.particle.GetForce();
                        soldier.particle.SetForce({ -CROUCHRUNSPEED, particle_force.y });
                    }
                }
                // Proning
            } else if ((soldier.legs_animation.GetType() == AnimationType::Prone) ||
                       (soldier.legs_animation.GetType() == AnimationType::ProneMove) ||
                       ((soldier.legs_animation.GetType() == AnimationType::GetUp) &&
                        (soldier.body_animation.GetType() != AnimationType::Throw) &&
                        (soldier.body_animation.GetType() != AnimationType::Punch))) {
                if (soldier.on_ground) {
                    if (((soldier.legs_animation.GetType() == AnimationType::Prone) &&
                         (soldier.legs_animation.GetFrame() > 25)) ||
                        (soldier.legs_animation.GetType() == AnimationType::ProneMove)) {
                        if (cleft || cright) {
                            if ((soldier.legs_animation.GetFrame() < 4) ||
                                (soldier.legs_animation.GetFrame() > 14)) {

                                glm::vec2 particle_force = soldier.particle.GetForce();
                                if (cleft) {
                                    particle_force.x = -PRONESPEED;
                                } else {
                                    particle_force.x = PRONESPEED;
                                }
                                soldier.particle.SetForce(particle_force);
                            }

                            LegsApplyAnimation(
                              soldier, animation_data_manager, AnimationType::ProneMove, 1);

                            if ((soldier.body_animation.GetType() != AnimationType::ClipIn) &&
                                (soldier.body_animation.GetType() != AnimationType::ClipOut) &&
                                (soldier.body_animation.GetType() != AnimationType::SlideBack) &&
                                (soldier.body_animation.GetType() != AnimationType::Reload) &&
                                (soldier.body_animation.GetType() != AnimationType::Change) &&
                                (soldier.body_animation.GetType() != AnimationType::Throw) &&
                                (soldier.body_animation.GetType() != AnimationType::ThrowWeapon)) {
                                BodyApplyAnimation(
                                  soldier, animation_data_manager, AnimationType::ProneMove, 1);
                            }

                            if (soldier.legs_animation.GetType() != AnimationType::ProneMove) {
                                soldier.legs_animation = AnimationState(
                                  animation_data_manager.Get(AnimationType::ProneMove));
                            }
                        } else {
                            if (soldier.legs_animation.GetType() != AnimationType::Prone) {
                                soldier.legs_animation =
                                  AnimationState(animation_data_manager.Get(AnimationType::Prone));
                            }
                            soldier.legs_animation.SetFrame(26);
                        }
                    }
                }
            } else if (cright && soldier.control.up) {
                if (soldier.on_ground) {
                    if ((soldier.legs_animation.GetType() == AnimationType::Run) ||
                        (soldier.legs_animation.GetType() == AnimationType::RunBack) ||
                        (soldier.legs_animation.GetType() == AnimationType::Stand) ||
                        (soldier.legs_animation.GetType() == AnimationType::Crouch) ||
                        (soldier.legs_animation.GetType() == AnimationType::CrouchRun) ||
                        (soldier.legs_animation.GetType() == AnimationType::CrouchRunBack)) {
                        LegsApplyAnimation(
                          soldier, animation_data_manager, AnimationType::JumpSide, 1);
                    }

                    if (soldier.legs_animation.GetFrame() ==
                        soldier.legs_animation.GetFramesCount()) {
                        LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Run, 1);
                    }
                } else if ((soldier.legs_animation.GetType() == AnimationType::Roll) ||
                           (soldier.legs_animation.GetType() == AnimationType::RollBack)) {
                    if (soldier.direction == 1) {
                        LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Run, 1);
                    } else {
                        LegsApplyAnimation(
                          soldier, animation_data_manager, AnimationType::RunBack, 1);
                    }
                }
                if (soldier.legs_animation.GetType() == AnimationType::Jump) {
                    if (soldier.legs_animation.GetFrame() < 10) {
                        LegsApplyAnimation(
                          soldier, animation_data_manager, AnimationType::JumpSide, 1);
                    }
                }

                if (soldier.legs_animation.GetType() == AnimationType::JumpSide) {
                    if ((soldier.legs_animation.GetFrame() > 3) &&
                        (soldier.legs_animation.GetFrame() < 11)) {
                        glm::vec2 particle_force = soldier.particle.GetForce();
                        particle_force.x = JUMPDIRSPEED;
                        particle_force.y = -JUMPDIRSPEED / 1.2F;
                        soldier.particle.SetForce(particle_force);
                    }
                }
            } else if (cleft && soldier.control.up) {
                if (soldier.on_ground) {
                    if ((soldier.legs_animation.GetType() == AnimationType::Run) ||
                        (soldier.legs_animation.GetType() == AnimationType::RunBack) ||
                        (soldier.legs_animation.GetType() == AnimationType::Stand) ||
                        (soldier.legs_animation.GetType() == AnimationType::Crouch) ||
                        (soldier.legs_animation.GetType() == AnimationType::CrouchRun) ||
                        (soldier.legs_animation.GetType() == AnimationType::CrouchRunBack)) {
                        LegsApplyAnimation(
                          soldier, animation_data_manager, AnimationType::JumpSide, 1);
                    }

                    if (soldier.legs_animation.GetFrame() ==
                        soldier.legs_animation.GetFramesCount()) {
                        LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Run, 1);
                    }
                } else if ((soldier.legs_animation.GetType() == AnimationType::Roll) ||
                           (soldier.legs_animation.GetType() == AnimationType::RollBack)) {
                    if (soldier.direction == -1) {
                        LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Run, 1);
                    } else {
                        LegsApplyAnimation(
                          soldier, animation_data_manager, AnimationType::RunBack, 1);
                    }
                }

                if (soldier.legs_animation.GetType() == AnimationType::Jump) {
                    if (soldier.legs_animation.GetFrame() < 10) {
                        LegsApplyAnimation(
                          soldier, animation_data_manager, AnimationType::JumpSide, 1);
                    }
                }

                if (soldier.legs_animation.GetType() == AnimationType::JumpSide) {
                    if ((soldier.legs_animation.GetFrame() > 3) &&
                        (soldier.legs_animation.GetFrame() < 11)) {
                        glm::vec2 particle_force = soldier.particle.GetForce();
                        particle_force.x = -JUMPDIRSPEED;
                        particle_force.y = -JUMPDIRSPEED / 1.2F;
                        soldier.particle.SetForce(particle_force);
                    }
                }
            } else if (soldier.control.up) {
                if (soldier.on_ground) {
                    if (soldier.legs_animation.GetType() != AnimationType::Jump) {
                        LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Jump, 1);
                    }
                    if (soldier.legs_animation.GetFrame() ==
                        soldier.legs_animation.GetFramesCount()) {
                        LegsApplyAnimation(
                          soldier, animation_data_manager, AnimationType::Stand, 1);
                    }
                }
                if (soldier.legs_animation.GetType() == AnimationType::Jump) {
                    if ((soldier.legs_animation.GetFrame() > 8) &&
                        (soldier.legs_animation.GetFrame() < 15)) {
                        glm::vec particle_force = soldier.particle.GetForce();
                        particle_force.y = -JUMPSPEED;
                        soldier.particle.SetForce(particle_force);
                    }
                    if (soldier.legs_animation.GetFrame() ==
                        soldier.legs_animation.GetFramesCount()) {
                        LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Fall, 1);
                    }
                }
            } else if (soldier.control.down) {
                if (soldier.on_ground) {
                    LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Crouch, 1);
                }
            } else if (cright) {
                if (true) {
                    // TODO
                    // if para = 0
                    if (soldier.direction == 1) {
                        LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Run, 1);
                    } else {
                        LegsApplyAnimation(
                          soldier, animation_data_manager, AnimationType::RunBack, 1);
                    }
                }

                if (soldier.on_ground) {
                    glm::vec2 particle_force = soldier.particle.GetForce();
                    particle_force.x = RUNSPEED;
                    particle_force.y = -RUNSPEEDUP;
                    soldier.particle.SetForce(particle_force);
                } else {
                    glm::vec2 particle_force = soldier.particle.GetForce();
                    particle_force.x = FLYSPEED;
                    soldier.particle.SetForce(particle_force);
                }
            } else if (cleft) {
                if (true) {
                    // if para = 0
                    if (soldier.direction == -1) {
                        LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Run, 1);
                    } else {
                        LegsApplyAnimation(
                          soldier, animation_data_manager, AnimationType::RunBack, 1);
                    }
                }

                glm::vec2 particle_force = soldier.particle.GetForce();
                if (soldier.on_ground) {
                    particle_force.x = -RUNSPEED;
                    particle_force.y = -RUNSPEEDUP;
                } else {
                    particle_force.x = -FLYSPEED;
                }
                soldier.particle.SetForce(particle_force);
            } else {
                if (soldier.on_ground) {
                    LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Stand, 1);
                } else {
                    LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Fall, 1);
                }
            }
        }
        // Body animations

        if ((soldier.legs_animation.GetType() == AnimationType::Roll) &&
            (soldier.body_animation.GetType() != AnimationType::Roll)) {
            BodyApplyAnimation(soldier, animation_data_manager, AnimationType::Roll, 1);
        }
        if ((soldier.body_animation.GetType() == AnimationType::Roll) &&
            (soldier.legs_animation.GetType() != AnimationType::Roll)) {
            LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Roll, 1);
        }
        if ((soldier.legs_animation.GetType() == AnimationType::RollBack) &&
            (soldier.body_animation.GetType() != AnimationType::RollBack)) {
            BodyApplyAnimation(soldier, animation_data_manager, AnimationType::RollBack, 1);
        }
        if ((soldier.body_animation.GetType() == AnimationType::RollBack) &&
            (soldier.legs_animation.GetType() != AnimationType::RollBack)) {
            LegsApplyAnimation(soldier, animation_data_manager, AnimationType::RollBack, 1);
        }

        if ((soldier.body_animation.GetType() == AnimationType::Roll) ||
            (soldier.body_animation.GetType() == AnimationType::RollBack)) {
            if (soldier.legs_animation.GetFrame() != soldier.body_animation.GetFrame()) {
                if (soldier.legs_animation.GetFrame() > soldier.body_animation.GetFrame()) {
                    soldier.body_animation.SetFrame(soldier.legs_animation.GetFrame());
                } else {
                    soldier.legs_animation.SetFrame(soldier.body_animation.GetFrame());
                }
            }
        }

        // Gracefully end a roll animation
        if (((soldier.body_animation.GetType() == AnimationType::Roll) ||
             (soldier.body_animation.GetType() == AnimationType::RollBack)) &&
            (soldier.body_animation.GetFrame() == soldier.body_animation.GetFramesCount())) {
            // Was probably a roll
            if (soldier.on_ground) {
                if (soldier.control.down) {
                    if (cleft || cright) {
                        if (soldier.body_animation.GetType() == AnimationType::Roll) {
                            LegsApplyAnimation(
                              soldier, animation_data_manager, AnimationType::CrouchRun, 1);
                        } else {
                            LegsApplyAnimation(
                              soldier, animation_data_manager, AnimationType::CrouchRunBack, 1);
                        }
                    } else {
                        LegsApplyAnimation(
                          soldier, animation_data_manager, AnimationType::Crouch, 15);
                    }
                }
                // Was probably a backflip
            } else if ((soldier.body_animation.GetType() == AnimationType::RollBack) &&
                       soldier.control.up) {
                if (cleft || cright) {
                    // Run back or forward depending on facing direction and direction key
                    // pressed
                    if ((soldier.direction == 1) ^ (cleft)) {
                        LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Run, 1);
                    } else {
                        LegsApplyAnimation(
                          soldier, animation_data_manager, AnimationType::RunBack, 1);
                    }
                } else {
                    LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Fall, 1);
                }
                // Was probably a roll (that ended mid-air)
            } else if (soldier.control.down) {
                if (cleft || cright) {
                    if (soldier.body_animation.GetType() == AnimationType::Roll) {
                        LegsApplyAnimation(
                          soldier, animation_data_manager, AnimationType::CrouchRun, 1);
                    } else {
                        LegsApplyAnimation(
                          soldier, animation_data_manager, AnimationType::CrouchRunBack, 1);
                    }
                } else {
                    LegsApplyAnimation(soldier, animation_data_manager, AnimationType::Crouch, 15);
                }
            }
            BodyApplyAnimation(soldier, animation_data_manager, AnimationType::Stand, 1);
        }

        if ((!soldier.control.throw_grenade &&
             (soldier.body_animation.GetType() != AnimationType::Recoil) &&
             (soldier.body_animation.GetType() != AnimationType::SmallRecoil) &&
             (soldier.body_animation.GetType() != AnimationType::AimRecoil) &&
             (soldier.body_animation.GetType() != AnimationType::HandsUpRecoil) &&
             (soldier.body_animation.GetType() != AnimationType::Shotgun) &&
             (soldier.body_animation.GetType() != AnimationType::Barret) &&
             (soldier.body_animation.GetType() != AnimationType::Change) &&
             (soldier.body_animation.GetType() != AnimationType::ThrowWeapon) &&
             (soldier.body_animation.GetType() != AnimationType::WeaponNone) &&
             (soldier.body_animation.GetType() != AnimationType::Punch) &&
             (soldier.body_animation.GetType() != AnimationType::Roll) &&
             (soldier.body_animation.GetType() != AnimationType::RollBack) &&
             (soldier.body_animation.GetType() != AnimationType::ReloadBow) &&
             (soldier.body_animation.GetType() != AnimationType::Cigar) &&
             (soldier.body_animation.GetType() != AnimationType::Match) &&
             (soldier.body_animation.GetType() != AnimationType::Smoke) &&
             (soldier.body_animation.GetType() != AnimationType::Wipe) &&
             (soldier.body_animation.GetType() != AnimationType::TakeOff) &&
             (soldier.body_animation.GetType() != AnimationType::Groin) &&
             (soldier.body_animation.GetType() != AnimationType::Piss) &&
             (soldier.body_animation.GetType() != AnimationType::Mercy) &&
             (soldier.body_animation.GetType() != AnimationType::Mercy2) &&
             (soldier.body_animation.GetType() != AnimationType::Victory) &&
             (soldier.body_animation.GetType() != AnimationType::Own) &&
             (soldier.body_animation.GetType() != AnimationType::Reload) &&
             (soldier.body_animation.GetType() != AnimationType::Prone) &&
             (soldier.body_animation.GetType() != AnimationType::GetUp) &&
             (soldier.body_animation.GetType() != AnimationType::ProneMove) &&
             (soldier.body_animation.GetType() != AnimationType::Melee)) ||
            ((soldier.body_animation.GetFrame() == soldier.body_animation.GetFramesCount()) &&
             (soldier.body_animation.GetType() != AnimationType::Prone))) {
            if (soldier.stance != STANCE_PRONE) {
                if (soldier.stance == STANCE_STAND) {
                    BodyApplyAnimation(soldier, animation_data_manager, AnimationType::Stand, 1);
                }

                if (soldier.stance == STANCE_CROUCH) {
                    if (soldier.collider_distance < 255) {
                        if (soldier.body_animation.GetType() == AnimationType::HandsUpRecoil) {
                            BodyApplyAnimation(
                              soldier, animation_data_manager, AnimationType::HandsUpAim, 11);
                        } else {
                            BodyApplyAnimation(
                              soldier, animation_data_manager, AnimationType::HandsUpAim, 1);
                        }
                    } else {
                        if (soldier.body_animation.GetType() == AnimationType::AimRecoil) {
                            BodyApplyAnimation(
                              soldier, animation_data_manager, AnimationType::Aim, 6);
                        } else {
                            BodyApplyAnimation(
                              soldier, animation_data_manager, AnimationType::Aim, 1);
                        }
                    }
                }
            } else {
                BodyApplyAnimation(soldier, animation_data_manager, AnimationType::Prone, 26);
            }
        }

        if ((soldier.legs_animation.GetType() == AnimationType::Crouch) ||
            (soldier.legs_animation.GetType() == AnimationType::CrouchRun) ||
            (soldier.legs_animation.GetType() == AnimationType::CrouchRunBack)) {
            soldier.stance = STANCE_CROUCH;
        } else {
            soldier.stance = STANCE_STAND;
        }
        if ((soldier.legs_animation.GetType() == AnimationType::Prone) ||
            (soldier.legs_animation.GetType() == AnimationType::ProneMove)) {
            soldier.stance = STANCE_PRONE;
        }
    }
}

void Update(State& state,
            Soldier& soldier,
            const AnimationDataManager& animation_data_manager,
            std::vector<BulletParams>& bullet_emitter)
{
    const Map& map = state.map;
    float body_y = 0.0f;
    float arm_s;

    soldier.particle.Euler();
    UpdateControl(state, soldier, animation_data_manager, bullet_emitter);

    RepositionSoldierSkeletonParts(soldier);

    for (int i = 1; i <= 20; i++) {
        if ((soldier.dead_meat || soldier.half_dead) && (i < 17) && (i != 7) && (i != 8)) {
            auto xy = soldier.particle.position;
            soldier.on_ground = CheckSkeletonMapCollision(soldier, map, i, xy.x, xy.y, state);
        }
    }

    if (!soldier.dead_meat) {
        soldier.body_animation.DoAnimation();
        soldier.legs_animation.DoAnimation();

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
                        if ((soldier.legs_animation.GetType() == AnimationType::Stand) ||
                            (soldier.legs_animation.GetType() == AnimationType::Crouch) ||
                            (soldier.legs_animation.GetType() == AnimationType::Prone) ||
                            (soldier.legs_animation.GetType() == AnimationType::ProneMove) ||
                            (soldier.legs_animation.GetType() == AnimationType::GetUp) ||
                            (soldier.legs_animation.GetType() == AnimationType::Fall) ||
                            (soldier.legs_animation.GetType() == AnimationType::Mercy) ||
                            (soldier.legs_animation.GetType() == AnimationType::Mercy2) ||
                            (soldier.legs_animation.GetType() == AnimationType::Own)) {
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
                                if ((soldier.legs_animation.GetType() == AnimationType::Stand) ||
                                    (soldier.legs_animation.GetType() == AnimationType::Fall) ||
                                    (soldier.legs_animation.GetType() == AnimationType::Crouch)) {
                                    soldier.particle.velocity_.x *= STANDSURFACECOEFX;
                                    soldier.particle.velocity_.y *= STANDSURFACECOEFY;

                                    glm::vec2 particle_force = soldier.particle.GetForce();
                                    particle_force.x -= soldier.particle.velocity_.x;
                                    soldier.particle.SetForce(particle_force);
                                } else if (soldier.legs_animation.GetType() ==
                                           AnimationType::Prone) {
                                    if (soldier.legs_animation.GetFrame() > 24) {
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
                                } else if (soldier.legs_animation.GetType() ==
                                           AnimationType::GetUp) {
                                    soldier.particle.velocity_.x *= SURFACECOEFX;
                                    soldier.particle.velocity_.y *= SURFACECOEFY;
                                } else if (soldier.legs_animation.GetType() ==
                                           AnimationType::ProneMove) {
                                    soldier.particle.velocity_.x *= STANDSURFACECOEFX;
                                    soldier.particle.velocity_.y *= STANDSURFACECOEFY;
                                }
                            }
                        } else if ((soldier.legs_animation.GetType() == AnimationType::CrouchRun) ||
                                   (soldier.legs_animation.GetType() ==
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
        soldier.body_animation.GetType() == AnimationType::Mercy ||
        soldier.body_animation.GetType() == AnimationType::Mercy2) {
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
                (((soldier.legs_animation.GetType() == AnimationType::Crouch &&
                   soldier.legs_animation.GetFrame() > 13) ||
                  soldier.legs_animation.GetType() == AnimationType::CrouchRun ||
                  soldier.legs_animation.GetType() == AnimationType::CrouchRunBack) ||
                 (soldier.legs_animation.GetType() == AnimationType::Prone &&
                  soldier.legs_animation.GetFrame() > 23))) {
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

void ControlThrowGrenade(Soldier& soldier, const AnimationDataManager& animation_data_manager)
{
    if (!soldier.control.throw_grenade) {
        soldier.grenade_can_throw = true;
    }

    if (soldier.grenade_can_throw && soldier.control.throw_grenade &&
        soldier.body_animation.GetType() != AnimationType::Roll &&
        soldier.body_animation.GetType() != AnimationType::RollBack) {

        BodyApplyAnimation(soldier, animation_data_manager, AnimationType::Throw, 1);
    }

    if (soldier.body_animation.GetType() == AnimationType::Throw &&
        (!soldier.control.throw_grenade || (soldier.body_animation.GetFrame() == 36))) {

        // Grenade throw
        if (soldier.body_animation.GetFrame() > 14 && soldier.body_animation.GetFrame() < 37 &&
            GetTertiaryWeapon(soldier).GetAmmoCount() > 0 /*&& CeaseFireCounter < 0*/) {

            // TODO: implement spawning grenade as a projectile
        }

        if (soldier.control.throw_grenade) {
            soldier.grenade_can_throw = false;
        }

        if (GetPrimaryWeapon(soldier).GetAmmoCount() == 0) {
            // TODO: implement stopping reloading of the weapon while throwing grenade
        }
    }
}
} // namespace Soldank::SoldierPhysics
