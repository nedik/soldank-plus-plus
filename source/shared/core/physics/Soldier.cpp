#include "Soldier.hpp"

#include "core/math/Calc.hpp"
#include "core/entities/Bullet.hpp"
#include "core/types/TeamType.hpp"
#include "core/types/WeaponType.hpp"
#include "core/entities/WeaponParametersFactory.hpp"

#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

const float SLIDELIMIT = 0.2F;
const float GRAV = 0.06F;
const float SURFACECOEFX = 0.970F;
const float SURFACECOEFY = 0.970F;
const float CROUCHMOVESURFACECOEFX = 0.85F;
const float CROUCHMOVESURFACECOEFY = 0.97F;
const float STANDSURFACECOEFX = 0.00F;
const float STANDSURFACECOEFY = 0.00F;

const std::uint8_t POS_STAND = 1;
const std::uint8_t POS_CROUCH = 2;
const std::uint8_t POS_PRONE = 3;

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

namespace Soldat
{
Soldier::Soldier(glm::vec2 spawn_position)
    : active(true)
    , dead_meat(false)
    , style(0)
    , num(1)
    , visible(1)
    , on_ground(false)
    , on_ground_for_law(false)
    , on_ground_last_frame(false)
    , on_ground_permanent(false)
    , direction(1)
    , old_direction(1)
    , health(150.0)
    , alpha(255.0)
    , jets_count(0)
    , jets_count_prev(0)
    , wear_helmet(0)
    , has_cigar(1)
    , vest(0.0)
    , idle_time(0)
    , idle_random(0)
    , position(0)
    , on_fire(0)
    , collider_distance(255)
    , half_dead(false)
    , skeleton(ParticleSystem::LoadFromFile("gostek.po", 4.5f, 1.0f, 1.06f * GRAV, 0.0f, 0.9945f))
    , legs_animation(AnimationType::Stand)
    , body_animation(AnimationType::Stand)
    , control()
    , active_weapon(0)
    , weapons{ { WeaponParametersFactory::GetParameters(WeaponType::DesertEagles, false) },
               { WeaponParametersFactory::GetParameters(WeaponType::Knife, false) },
               { WeaponParametersFactory::GetParameters(WeaponType::FragGrenade, false) } }
    , fired(0)
    , particle(true,
               spawn_position,
               spawn_position,
               glm::vec2(0.0f, 0.0f),
               glm::vec2(0.0f, 0.0f),
               1.0f,
               1.0f,
               GRAV,
               0.99,
               0.0f)
{
}

void Soldier::SwitchWeapon()
{
    int new_active_weapon = (active_weapon + 1) % 2;
    active_weapon = new_active_weapon;
    // weapons[new_active_weapon].start_up_time_count =
    //   weapons[new_active_weapon].GetWeaponParameters().start_up_time;
    weapons[new_active_weapon].ResetStartUpTimeCount();
    // weapons[new_active_weapon].reload_time_prev = weapons[new_active_weapon].reload_time_count;
    weapons[new_active_weapon].SetReloadTimePrev(weapons[new_active_weapon].GetReloadTimeCount());
}

void Soldier::UpdateKeys(const Control& _control)
{
    control = _control;
}

void Soldier::LegsApplyAnimation(AnimationType id, unsigned int frame)
{
    if (!legs_animation.IsAny({ AnimationType::Prone, AnimationType::ProneMove }) &&
        legs_animation.GetType() != id) {
        legs_animation = AnimationState(id);
        legs_animation.SetFrame(frame);
    }
}

void Soldier::BodyApplyAnimation(AnimationType id, unsigned int frame)
{
    if (body_animation.GetType() != id) {
        body_animation = AnimationState(id);
        body_animation.SetFrame(frame);
    }
}

void Soldier::HandleSpecialPolytypes(const Map& map, PMSPolygonType polytype, glm::vec2 _pos)
{
    if (polytype == PMSPolygonType::Deadly || polytype == PMSPolygonType::BloodyDeadly ||
        polytype == PMSPolygonType::Explosive) {
        particle.position = glm::vec2(map.GetSpawnPoints()[0].x, map.GetSpawnPoints()[0].y);
    }
}

void Soldier::UpdateControl(State& state, std::vector<BulletParams>& bullet_emitter)
{
    bool player_pressed_left_right = false;

    if (legs_animation.GetSpeed() < 1) {
        legs_animation.SetSpeed(1);
    }

    if (body_animation.GetSpeed() < 1) {
        body_animation.SetSpeed(1);
    }

    control.mouse_aim_x = (state.mouse.x - (float)state.game_width / 2.0 + state.camera.x);
    control.mouse_aim_y = (state.mouse.y - (float)state.game_height / 2.0 + state.camera.y);

    auto cleft = control.left;
    auto cright = control.right;

    // If both left and right directions are pressed, then decide which direction
    // to go in
    if (cleft && cright) {
        // Remember that both directions were pressed, as it's useful for some moves
        player_pressed_left_right = true;

        if (control.was_jumping) {
            // If jumping, keep going in the old direction
            if (control.was_running_left) {
                cright = false;
            } else {
                cleft = false;
            }
        } else {
            // If not jumping, instead go in the new direction
            if (control.was_running_left) {
                cleft = false;
            } else {
                cright = false;
            }
        }
    } else {
        control.was_running_left = cleft;
        control.was_jumping = control.up;
    }

    auto conflicting_keys_pressed = [](const Control& c) {
        return ((int)c.grenade + (int)c.change + (int)c.throw_grenade + (int)c.reload) > 1;
    };

    // Handle simultaneous key presses that would conflict
    if (conflicting_keys_pressed(control)) {
        // At least two buttons pressed, so deactivate any previous one
        if (control.was_throwing_grenade) {
            control.grenade = false;
        } else if (control.was_changing_weapon) {
            control.change = false;
        } else if (control.was_throwing_weapon) {
            control.throw_grenade = false;
        } else if (control.was_reloading_weapon) {
            control.reload = false;
        }

        // If simultaneously pressing two or more new buttons, then deactivate them
        // in order of least preference
        while (conflicting_keys_pressed(control)) {
            if (control.reload) {
                control.reload = false;
            } else if (control.change) {
                control.change = false;
            } else if (control.throw_grenade) {
                control.throw_grenade = false;
            } else if (control.grenade) {
                control.grenade = false;
            }
        }
    } else {
        control.was_throwing_grenade = control.grenade;
        control.was_changing_weapon = control.change;
        control.was_throwing_weapon = control.throw_grenade;
        control.was_reloading_weapon = control.reload;
    }

    if (dead_meat) {
        // TODO: co to free controls?
        // control.free_controls();
    }

    // self.fired = 0;
    control.mouse_aim_x = (int)((float)control.mouse_aim_x + particle.GetVelocity().x);
    control.mouse_aim_y = (int)((float)control.mouse_aim_y + particle.GetVelocity().y);

    if (control.jets && (((legs_animation.GetType() == AnimationType::JumpSide) &&
                          (((direction == -1) && cright) || ((direction == 1) && cleft) ||
                           player_pressed_left_right)) ||
                         ((legs_animation.GetType() == AnimationType::RollBack) && control.up))) {
        BodyApplyAnimation(AnimationType::RollBack, 1);
        LegsApplyAnimation(AnimationType::RollBack, 1);
    } else if (control.jets && (jets_count > 0)) {
        if (on_ground) {
            // particle.force.y = -2.5 * (state.gravity > 0.05 ? JETSPEED :
            // state.gravity * 2.0);
            glm::vec2 particle_force = particle.GetForce();
            if (state.gravity > 0.05) {
                particle.SetForce({ particle_force.x, -2.5F * JETSPEED });
            } else {
                particle.SetForce({ particle_force.x, -2.5 * (state.gravity * 2.0F) });
            }
        } else if (position != POS_PRONE) {
            // particle.force.y -= (state.gravity > 0.05 ? JETSPEED : state.gravity
            // * 2.0);
            glm::vec2 particle_force = particle.GetForce();
            if (state.gravity > 0.05) {
                particle.SetForce({ particle_force.x, particle_force.y - JETSPEED });
            } else {
                particle.SetForce({ particle_force.x, particle_force.y - state.gravity * 2.0 });
            }
        } else {
            glm::vec2 particle_force = particle.GetForce();
            particle.SetForce(
              { particle_force.x +
                  (float)direction * (state.gravity > 0.05 ? JETSPEED / 2.0 : state.gravity),
                particle_force.y });
        }

        if ((legs_animation.GetType() != AnimationType::GetUp) &&
            (body_animation.GetType() != AnimationType::Roll) &&
            (body_animation.GetType() != AnimationType::RollBack)) {
            LegsApplyAnimation(AnimationType::Fall, 1);
        }
        jets_count -= 1;
    }

    // FIRE!!!!
    if (GetPrimaryWeapon().GetWeaponParameters().kind == WeaponType::Chainsaw ||
        (body_animation.GetType() != AnimationType::Roll) &&
          (body_animation.GetType() != AnimationType::RollBack) &&
          (body_animation.GetType() != AnimationType::Melee) &&
          (body_animation.GetType() != AnimationType::Change)) {
        if (((body_animation.GetType() == AnimationType::HandsUpAim) &&
             (body_animation.GetFrame() == 11)) ||
            (body_animation.GetType() != AnimationType::HandsUpAim)) {
            if (control.fire)
            // and (SpriteC.CeaseFireCounter < 0)
            {
                if (GetPrimaryWeapon().GetWeaponParameters().kind == WeaponType::NoWeapon ||
                    GetPrimaryWeapon().GetWeaponParameters().kind == WeaponType::Knife) {
                    BodyApplyAnimation(AnimationType::Punch, 1);
                } else {
                    Fire(bullet_emitter);
                    control.fire = false;
                }
            }
        }
    }

    // change weapon animation
    if ((body_animation.GetType() != AnimationType::Roll) &&
        (body_animation.GetType() != AnimationType::RollBack)) {
        if (control.change) {
            BodyApplyAnimation(AnimationType::Change, 1);
        }
    }

    // change weapon
    if (body_animation.GetType() == AnimationType::Change) {
        if (body_animation.GetFrame() == 2) {
            // TODO: play sound
            body_animation.SetNextFrame();
        } else if (body_animation.GetFrame() == 25) {
            SwitchWeapon();
        } else if ((body_animation.GetFrame() ==
                    Animations::Get(AnimationType::Change).GetFrames().size()) &&
                   (GetPrimaryWeapon().GetAmmoCount() == 0)) {
            BodyApplyAnimation(AnimationType::Stand, 1);
        }
    }

    // throw weapon
    if (control.drop &&
        (body_animation.GetType() != AnimationType::Change || body_animation.GetFrame() > 25) &&
        !body_animation.IsAny(
          { AnimationType::Roll, AnimationType::RollBack, AnimationType::ThrowWeapon })
        // && !flamegod bonus
        && !GetPrimaryWeapon().IsAny({
             WeaponType::Bow,
             WeaponType::FlameBow,
             WeaponType::NoWeapon,
           })) {
        BodyApplyAnimation(AnimationType::ThrowWeapon, 1);

        if (GetPrimaryWeapon().GetWeaponParameters().kind == WeaponType::Knife) {
            body_animation.SetSpeed(2);
        }
    }

    // throw knife
    if (body_animation.GetType() == AnimationType::ThrowWeapon &&
        GetPrimaryWeapon().GetWeaponParameters().kind == WeaponType::Knife &&
        (!control.drop || body_animation.GetFrame() == 16)) {

        Weapon weapon{ WeaponParametersFactory::GetParameters(WeaponType::ThrownKnife, false) };
        auto aim_x = (float)control.mouse_aim_x;
        auto aim_y = (float)control.mouse_aim_y;
        auto dir = Calc::Vec2Normalize(glm::vec2(aim_x, aim_y) - skeleton->GetPos(15));
        auto frame = (float)body_animation.GetFrame();
        auto thrown_mul = 1.5F * min(16.0F, max(8.0F, frame)) / 16.0F;
        auto bullet_vel = dir * weapon.GetWeaponParameters().speed * thrown_mul;
        auto inherited_vel =
          particle.GetVelocity() * weapon.GetWeaponParameters().inherited_velocity;
        auto velocity = bullet_vel + inherited_vel;

        BulletParams params{
            weapon.GetWeaponParameters().bullet_style,
            weapon.GetWeaponParameters().kind,
            skeleton->GetPos(16) + velocity,
            velocity,
            (std::int16_t)weapon.GetWeaponParameters().timeout,
            weapon.GetWeaponParameters().hit_multiply,
            TeamType::None,
        };
        bullet_emitter.push_back(params);

        control.drop = false;
        BodyApplyAnimation(AnimationType::Stand, 1);
    }

    // Punch!
    if (!dead_meat) {
        if ((body_animation.GetType() == AnimationType::Punch) &&
            (body_animation.GetFrame() == 11)) {
            body_animation.SetNextFrame();
        }
    }

    // Buttstock!
    if (dead_meat) {
        if ((body_animation.GetType() == AnimationType::Melee) &&
            (body_animation.GetFrame() == 12)) {
            // weapons
        }
    }

    if (body_animation.GetType() == AnimationType::Melee && body_animation.GetFrame() > 20) {
        BodyApplyAnimation(AnimationType::Stand, 1);
    }

    // Prone
    if (control.prone) {
        if ((legs_animation.GetType() != AnimationType::GetUp) &&
            (legs_animation.GetType() != AnimationType::Prone) &&
            (legs_animation.GetType() != AnimationType::ProneMove)) {
            LegsApplyAnimation(AnimationType::Prone, 1);
            if ((body_animation.GetType() != AnimationType::Reload) &&
                (body_animation.GetType() != AnimationType::Change) &&
                (body_animation.GetType() != AnimationType::ThrowWeapon)) {
                BodyApplyAnimation(AnimationType::Prone, 1);
            }
            old_direction = direction;
            control.prone = false;
        }
    }

    // Get up
    if (position == POS_PRONE) {
        if (control.prone || (direction != old_direction)) {
            if (((legs_animation.GetType() == AnimationType::Prone) &&
                 (legs_animation.GetFrame() > 23)) ||
                (legs_animation.GetType() == AnimationType::ProneMove)) {
                if (legs_animation.GetType() != AnimationType::GetUp) {
                    legs_animation = { AnimationType::GetUp };
                    legs_animation.SetFrame(9);
                    control.prone = false;
                }
                if ((body_animation.GetType() != AnimationType::Reload) &&
                    (body_animation.GetType() != AnimationType::Change) &&
                    (body_animation.GetType() != AnimationType::ThrowWeapon)) {
                    BodyApplyAnimation(AnimationType::GetUp, 9);
                }
            }
        }
    }

    bool unprone = false;
    // Immediately switch from unprone to jump/sidejump, because the end of the
    // unprone animation can be seen as the "wind up" for the jump
    if ((legs_animation.GetType() == AnimationType::GetUp) &&
        (legs_animation.GetFrame() > 23 - (4 - 1)) && on_ground && control.up &&
        (cright || cleft)) {
        // Set sidejump frame 1 to 4 depending on which unprone frame we're in
        auto id = legs_animation.GetFrame() - (23 - (4 - 1));
        LegsApplyAnimation(AnimationType::JumpSide, id);
        unprone = true;
    } else if ((legs_animation.GetType() == AnimationType::GetUp) &&
               (legs_animation.GetFrame() > 23 - (4 - 1)) && on_ground && control.up &&
               !(cright || cleft)) {
        // Set jump frame 6 to 9 depending on which unprone frame we're in
        auto id = legs_animation.GetFrame() - (23 - (9 - 1));
        LegsApplyAnimation(AnimationType::Jump, id);
        unprone = true;
    } else if ((legs_animation.GetType() == AnimationType::GetUp) &&
               (legs_animation.GetFrame() > 23)) {
        if (cright || cleft) {
            if ((direction == 1) ^ cleft) {
                LegsApplyAnimation(AnimationType::Run, 1);
            } else {
                LegsApplyAnimation(AnimationType::RunBack, 1);
            }
        } else if (!on_ground && control.up) {
            LegsApplyAnimation(AnimationType::Run, 1);
        } else {
            LegsApplyAnimation(AnimationType::Stand, 1);
        }
        unprone = true;
    }

    if (unprone) {
        position = POS_STAND;

        if ((body_animation.GetType() != AnimationType::Reload) &&
            (body_animation.GetType() != AnimationType::Change) &&
            (body_animation.GetType() != AnimationType::ThrowWeapon)) {
            BodyApplyAnimation(AnimationType::Stand, 1);
        }
    }

    if (true) {
        // self.stat == 0 {
        if (((body_animation.GetType() == AnimationType::Stand) &&
             (legs_animation.GetType() == AnimationType::Stand) && !dead_meat && (idle_time > 0)) ||
            (idle_time > DEFAULT_IDLETIME)) {
            if (idle_random >= 0) {
                idle_time -= 1;
            }
        } else {
            idle_time = DEFAULT_IDLETIME;
        }

        if (idle_random == 0) {
            if (idle_time == 0) {
                BodyApplyAnimation(AnimationType::Smoke, 1);
                idle_time = DEFAULT_IDLETIME;
            }

            if ((body_animation.GetType() == AnimationType::Smoke) &&
                (body_animation.GetFrame() == 17)) {
                body_animation.SetNextFrame();
            }

            if (!dead_meat) {
                if ((idle_time == 1) && (body_animation.GetType() != AnimationType::Smoke) &&
                    (legs_animation.GetType() == AnimationType::Stand)) {
                    idle_time = DEFAULT_IDLETIME;
                    idle_random = -1;
                }
            }
        }

        // *CHEAT*
        if (legs_animation.GetSpeed() > 1) {
            if ((legs_animation.GetType() == AnimationType::Jump) ||
                (legs_animation.GetType() == AnimationType::JumpSide) ||
                (legs_animation.GetType() == AnimationType::Roll) ||
                (legs_animation.GetType() == AnimationType::RollBack) ||
                (legs_animation.GetType() == AnimationType::Prone) ||
                (legs_animation.GetType() == AnimationType::Run) ||
                (legs_animation.GetType() == AnimationType::RunBack)) {
                particle.velocity_.x /= (float)legs_animation.GetSpeed();
                particle.velocity_.y /= (float)legs_animation.GetSpeed();
            }

            if (legs_animation.GetSpeed() > 2) {
                if ((legs_animation.GetType() == AnimationType::ProneMove) ||
                    (legs_animation.GetType() == AnimationType::CrouchRun)) {
                    particle.velocity_.x /= (float)legs_animation.GetSpeed();
                    particle.velocity_.y /= (float)legs_animation.GetSpeed();
                }
            }
        }

        // TODO: Check if near collider

        // TODO if targetmode > freecontrols
        // End any ongoing idle animations if a key is pressed
        if ((body_animation.GetType() == AnimationType::Cigar) ||
            (body_animation.GetType() == AnimationType::Match) ||
            (body_animation.GetType() == AnimationType::Smoke) ||
            (body_animation.GetType() == AnimationType::Wipe) ||
            (body_animation.GetType() == AnimationType::Groin)) {
            if (cleft || cright || control.up || control.down || control.fire || control.jets ||
                control.grenade || control.change || control.change // TODO: 2x control.change
                || control.throw_grenade || control.reload || control.prone) {
                body_animation.SetFrame(Animations::Get(body_animation.GetType())
                                          .GetFrames()
                                          .size()); // TODO: Check this if correct
            }
        }

        // make anims out of controls
        // rolling
        if ((body_animation.GetType() != AnimationType::TakeOff) &&
            (body_animation.GetType() != AnimationType::Piss) &&
            (body_animation.GetType() != AnimationType::Mercy) &&
            (body_animation.GetType() != AnimationType::Mercy2) &&
            (body_animation.GetType() != AnimationType::Victory) &&
            (body_animation.GetType() != AnimationType::Own)) {
            if ((body_animation.GetType() == AnimationType::Roll) ||
                (body_animation.GetType() == AnimationType::RollBack)) {
                if (legs_animation.GetType() == AnimationType::Roll) {
                    if (on_ground) {
                        glm::vec2 particle_force = particle.GetForce();
                        particle.SetForce({ (float)direction * ROLLSPEED, particle_force.y });
                    } else {
                        glm::vec2 particle_force = particle.GetForce();
                        particle.SetForce({ (float)direction * 2.0F * FLYSPEED, particle_force.y });
                    }
                } else if (legs_animation.GetType() == AnimationType::RollBack) {
                    if (on_ground) {
                        glm::vec2 particle_force = particle.GetForce();
                        particle_force.x = (-(float)direction) * ROLLSPEED;
                        particle.SetForce(particle_force);
                    } else {
                        glm::vec2 particle_force = particle.GetForce();
                        particle_force.x = (-(float)(direction)) * 2.0F * FLYSPEED;
                        particle.SetForce(particle_force);
                    }
                    // if appropriate frames to move
                    if ((legs_animation.GetFrame() > 1) && (legs_animation.GetFrame() < 8)) {
                        if (control.up) {
                            glm::vec2 particle_force = particle.GetForce();
                            particle_force.y -= JUMPDIRSPEED * 1.5F;
                            particle_force.x *= 0.5;
                            particle.SetForce(particle_force);
                            particle.velocity_.x *= 0.8;
                        }
                    }
                }
                // downright
            } else if ((cright) && (control.down)) {
                if (on_ground) {
                    // roll to the side
                    if ((legs_animation.GetType() == AnimationType::Run) ||
                        (legs_animation.GetType() == AnimationType::RunBack) ||
                        (legs_animation.GetType() == AnimationType::Fall) ||
                        (legs_animation.GetType() == AnimationType::ProneMove) ||
                        ((legs_animation.GetType() == AnimationType::Prone) &&
                         (legs_animation.GetFrame() >= 24))) {
                        if ((legs_animation.GetType() == AnimationType::ProneMove) ||
                            ((legs_animation.GetType() == AnimationType::Prone) &&
                             (legs_animation.GetFrame() == legs_animation.GetFramesCount()))) {
                            control.prone = false;
                            position = POS_STAND;
                        }

                        if (direction == 1) {
                            BodyApplyAnimation(AnimationType::Roll, 1);
                            legs_animation = { AnimationType::Roll };
                            legs_animation.SetFrame(1);
                        } else {
                            BodyApplyAnimation(AnimationType::RollBack, 1);
                            legs_animation = { AnimationType::RollBack };
                            legs_animation.SetFrame(1);
                        }
                    } else {
                        if (direction == 1) {
                            LegsApplyAnimation(AnimationType::CrouchRun, 1);
                        } else {
                            LegsApplyAnimation(AnimationType::CrouchRunBack, 1);
                        }
                    }

                    if ((legs_animation.GetType() == AnimationType::CrouchRun) ||
                        (legs_animation.GetType() == AnimationType::CrouchRunBack)) {
                        glm::vec2 particle_force = particle.GetForce();
                        particle.SetForce({ CROUCHRUNSPEED, particle_force.y });
                    } else if ((legs_animation.GetType() == AnimationType::Roll) ||
                               (legs_animation.GetType() == AnimationType::RollBack)) {
                        glm::vec2 particle_force = particle.GetForce();
                        particle.SetForce({ 2.0F * CROUCHRUNSPEED, particle_force.y });
                    }
                }
                // downleft
            } else if (cleft && control.down) {
                if (on_ground) {
                    // roll to the side
                    if ((legs_animation.GetType() == AnimationType::Run) ||
                        (legs_animation.GetType() == AnimationType::RunBack) ||
                        (legs_animation.GetType() == AnimationType::Fall) ||
                        (legs_animation.GetType() == AnimationType::ProneMove) ||
                        ((legs_animation.GetType() == AnimationType::Prone) &&
                         (legs_animation.GetFrame() >= 24))) {
                        if ((legs_animation.GetType() == AnimationType::ProneMove) ||
                            ((legs_animation.GetType() == AnimationType::Prone) &&
                             (legs_animation.GetFrame() == legs_animation.GetFramesCount()))) {
                            control.prone = false;
                            position = POS_STAND;
                        }

                        if (direction == 1) {
                            BodyApplyAnimation(AnimationType::RollBack, 1);
                            legs_animation = { AnimationType::RollBack };
                            legs_animation.SetFrame(1);
                        } else {
                            BodyApplyAnimation(AnimationType::Roll, 1);
                            legs_animation = { AnimationType::Roll };
                            legs_animation.SetFrame(1);
                        }
                    } else {
                        if (direction == 1) {
                            LegsApplyAnimation(AnimationType::CrouchRunBack, 1);
                        } else {
                            LegsApplyAnimation(AnimationType::CrouchRun, 1);
                        }
                    }

                    if ((legs_animation.GetType() == AnimationType::CrouchRun) ||
                        (legs_animation.GetType() == AnimationType::CrouchRunBack)) {
                        glm::vec2 particle_force = particle.GetForce();
                        particle.SetForce({ -CROUCHRUNSPEED, particle_force.y });
                    }
                }
                // Proning
            } else if ((legs_animation.GetType() == AnimationType::Prone) ||
                       (legs_animation.GetType() == AnimationType::ProneMove) ||
                       ((legs_animation.GetType() == AnimationType::GetUp) &&
                        (body_animation.GetType() != AnimationType::Throw) &&
                        (body_animation.GetType() != AnimationType::Punch))) {
                if (on_ground) {
                    if (((legs_animation.GetType() == AnimationType::Prone) &&
                         (legs_animation.GetFrame() > 25)) ||
                        (legs_animation.GetType() == AnimationType::ProneMove)) {
                        if (cleft || cright) {
                            if ((legs_animation.GetFrame() < 4) ||
                                (legs_animation.GetFrame() > 14)) {

                                glm::vec2 particle_force = particle.GetForce();
                                if (cleft) {
                                    particle_force.x = -PRONESPEED;
                                } else {
                                    particle_force.x = PRONESPEED;
                                }
                                particle.SetForce(particle_force);
                            }

                            LegsApplyAnimation(AnimationType::ProneMove, 1);

                            if ((body_animation.GetType() != AnimationType::ClipIn) &&
                                (body_animation.GetType() != AnimationType::ClipOut) &&
                                (body_animation.GetType() != AnimationType::SlideBack) &&
                                (body_animation.GetType() != AnimationType::Reload) &&
                                (body_animation.GetType() != AnimationType::Change) &&
                                (body_animation.GetType() != AnimationType::Throw) &&
                                (body_animation.GetType() != AnimationType::ThrowWeapon)) {
                                BodyApplyAnimation(AnimationType::ProneMove, 1);
                            }

                            if (legs_animation.GetType() != AnimationType::ProneMove) {
                                legs_animation = AnimationState(AnimationType::ProneMove);
                            }
                        } else {
                            if (legs_animation.GetType() != AnimationType::Prone) {
                                legs_animation = AnimationState(AnimationType::Prone);
                            }
                            legs_animation.SetFrame(26);
                        }
                    }
                }
            } else if (cright && control.up) {
                if (on_ground) {
                    if ((legs_animation.GetType() == AnimationType::Run) ||
                        (legs_animation.GetType() == AnimationType::RunBack) ||
                        (legs_animation.GetType() == AnimationType::Stand) ||
                        (legs_animation.GetType() == AnimationType::Crouch) ||
                        (legs_animation.GetType() == AnimationType::CrouchRun) ||
                        (legs_animation.GetType() == AnimationType::CrouchRunBack)) {
                        LegsApplyAnimation(AnimationType::JumpSide, 1);
                    }

                    if (legs_animation.GetFrame() == legs_animation.GetFramesCount()) {
                        LegsApplyAnimation(AnimationType::Run, 1);
                    }
                } else if ((legs_animation.GetType() == AnimationType::Roll) ||
                           (legs_animation.GetType() == AnimationType::RollBack)) {
                    if (direction == 1) {
                        LegsApplyAnimation(AnimationType::Run, 1);
                    } else {
                        LegsApplyAnimation(AnimationType::RunBack, 1);
                    }
                }
                if (legs_animation.GetType() == AnimationType::Jump) {
                    if (legs_animation.GetFrame() < 10) {
                        LegsApplyAnimation(AnimationType::JumpSide, 1);
                    }
                }

                if (legs_animation.GetType() == AnimationType::JumpSide) {
                    if ((legs_animation.GetFrame() > 3) && (legs_animation.GetFrame() < 11)) {
                        glm::vec2 particle_force = particle.GetForce();
                        particle_force.x = JUMPDIRSPEED;
                        particle_force.y = -JUMPDIRSPEED / 1.2F;
                        particle.SetForce(particle_force);
                    }
                }
            } else if (cleft && control.up) {
                if (on_ground) {
                    if ((legs_animation.GetType() == AnimationType::Run) ||
                        (legs_animation.GetType() == AnimationType::RunBack) ||
                        (legs_animation.GetType() == AnimationType::Stand) ||
                        (legs_animation.GetType() == AnimationType::Crouch) ||
                        (legs_animation.GetType() == AnimationType::CrouchRun) ||
                        (legs_animation.GetType() == AnimationType::CrouchRunBack)) {
                        LegsApplyAnimation(AnimationType::JumpSide, 1);
                    }

                    if (legs_animation.GetFrame() == legs_animation.GetFramesCount()) {
                        LegsApplyAnimation(AnimationType::Run, 1);
                    }
                } else if ((legs_animation.GetType() == AnimationType::Roll) ||
                           (legs_animation.GetType() == AnimationType::RollBack)) {
                    if (direction == -1) {
                        LegsApplyAnimation(AnimationType::Run, 1);
                    } else {
                        LegsApplyAnimation(AnimationType::RunBack, 1);
                    }
                }

                if (legs_animation.GetType() == AnimationType::Jump) {
                    if (legs_animation.GetFrame() < 10) {
                        LegsApplyAnimation(AnimationType::JumpSide, 1);
                    }
                }

                if (legs_animation.GetType() == AnimationType::JumpSide) {
                    if ((legs_animation.GetFrame() > 3) && (legs_animation.GetFrame() < 11)) {
                        glm::vec2 particle_force = particle.GetForce();
                        particle_force.x = -JUMPDIRSPEED;
                        particle_force.y = -JUMPDIRSPEED / 1.2F;
                        particle.SetForce(particle_force);
                    }
                }
            } else if (control.up) {
                if (on_ground) {
                    if (legs_animation.GetType() != AnimationType::Jump) {
                        LegsApplyAnimation(AnimationType::Jump, 1);
                    }
                    if (legs_animation.GetFrame() == legs_animation.GetFramesCount()) {
                        LegsApplyAnimation(AnimationType::Stand, 1);
                    }
                }
                if (legs_animation.GetType() == AnimationType::Jump) {
                    if ((legs_animation.GetFrame() > 8) && (legs_animation.GetFrame() < 15)) {
                        glm::vec particle_force = particle.GetForce();
                        particle_force.y = -JUMPSPEED;
                        particle.SetForce(particle_force);
                    }
                    if (legs_animation.GetFrame() == legs_animation.GetFramesCount()) {
                        LegsApplyAnimation(AnimationType::Fall, 1);
                    }
                }
            } else if (control.down) {
                if (on_ground) {
                    LegsApplyAnimation(AnimationType::Crouch, 1);
                }
            } else if (cright) {
                if (true) {
                    // TODO
                    // if para = 0
                    if (direction == 1) {
                        LegsApplyAnimation(AnimationType::Run, 1);
                    } else {
                        LegsApplyAnimation(AnimationType::RunBack, 1);
                    }
                }

                if (on_ground) {
                    glm::vec2 particle_force = particle.GetForce();
                    particle_force.x = RUNSPEED;
                    particle_force.y = -RUNSPEEDUP;
                    particle.SetForce(particle_force);
                } else {
                    glm::vec2 particle_force = particle.GetForce();
                    particle_force.x = FLYSPEED;
                    particle.SetForce(particle_force);
                }
            } else if (cleft) {
                if (true) {
                    // if para = 0
                    if (direction == -1) {
                        LegsApplyAnimation(AnimationType::Run, 1);
                    } else {
                        LegsApplyAnimation(AnimationType::RunBack, 1);
                    }
                }

                glm::vec2 particle_force = particle.GetForce();
                if (on_ground) {
                    particle_force.x = -RUNSPEED;
                    particle_force.y = -RUNSPEEDUP;
                } else {
                    particle_force.x = -FLYSPEED;
                }
                particle.SetForce(particle_force);
            } else {
                if (on_ground) {
                    LegsApplyAnimation(AnimationType::Stand, 1);
                } else {
                    LegsApplyAnimation(AnimationType::Fall, 1);
                }
            }
        }
        // Body animations

        if ((legs_animation.GetType() == AnimationType::Roll) &&
            (body_animation.GetType() != AnimationType::Roll)) {
            BodyApplyAnimation(AnimationType::Roll, 1);
        }
        if ((body_animation.GetType() == AnimationType::Roll) &&
            (legs_animation.GetType() != AnimationType::Roll)) {
            LegsApplyAnimation(AnimationType::Roll, 1);
        }
        if ((legs_animation.GetType() == AnimationType::RollBack) &&
            (body_animation.GetType() != AnimationType::RollBack)) {
            BodyApplyAnimation(AnimationType::RollBack, 1);
        }
        if ((body_animation.GetType() == AnimationType::RollBack) &&
            (legs_animation.GetType() != AnimationType::RollBack)) {
            LegsApplyAnimation(AnimationType::RollBack, 1);
        }

        if ((body_animation.GetType() == AnimationType::Roll) ||
            (body_animation.GetType() == AnimationType::RollBack)) {
            if (legs_animation.GetFrame() != body_animation.GetFrame()) {
                if (legs_animation.GetFrame() > body_animation.GetFrame()) {
                    body_animation.SetFrame(legs_animation.GetFrame());
                } else {
                    legs_animation.SetFrame(body_animation.GetFrame());
                }
            }
        }

        // Gracefully end a roll animation
        if (((body_animation.GetType() == AnimationType::Roll) ||
             (body_animation.GetType() == AnimationType::RollBack)) &&
            (body_animation.GetFrame() == body_animation.GetFramesCount())) {
            // Was probably a roll
            if (on_ground) {
                if (control.down) {
                    if (cleft || cright) {
                        if (body_animation.GetType() == AnimationType::Roll) {
                            LegsApplyAnimation(AnimationType::CrouchRun, 1);
                        } else {
                            LegsApplyAnimation(AnimationType::CrouchRunBack, 1);
                        }
                    } else {
                        LegsApplyAnimation(AnimationType::Crouch, 15);
                    }
                }
                // Was probably a backflip
            } else if ((body_animation.GetType() == AnimationType::RollBack) && control.up) {
                if (cleft || cright) {
                    // Run back or forward depending on facing direction and direction key
                    // pressed
                    if ((direction == 1) ^ (cleft)) {
                        LegsApplyAnimation(AnimationType::Run, 1);
                    } else {
                        LegsApplyAnimation(AnimationType::RunBack, 1);
                    }
                } else {
                    LegsApplyAnimation(AnimationType::Fall, 1);
                }
                // Was probably a roll (that ended mid-air)
            } else if (control.down) {
                if (cleft || cright) {
                    if (body_animation.GetType() == AnimationType::Roll) {
                        LegsApplyAnimation(AnimationType::CrouchRun, 1);
                    } else {
                        LegsApplyAnimation(AnimationType::CrouchRunBack, 1);
                    }
                } else {
                    LegsApplyAnimation(AnimationType::Crouch, 15);
                }
            }
            BodyApplyAnimation(AnimationType::Stand, 1);
        }

        if ((!control.grenade && (body_animation.GetType() != AnimationType::Recoil) &&
             (body_animation.GetType() != AnimationType::SmallRecoil) &&
             (body_animation.GetType() != AnimationType::AimRecoil) &&
             (body_animation.GetType() != AnimationType::HandsUpRecoil) &&
             (body_animation.GetType() != AnimationType::Shotgun) &&
             (body_animation.GetType() != AnimationType::Barret) &&
             (body_animation.GetType() != AnimationType::Change) &&
             (body_animation.GetType() != AnimationType::ThrowWeapon) &&
             (body_animation.GetType() != AnimationType::WeaponNone) &&
             (body_animation.GetType() != AnimationType::Punch) &&
             (body_animation.GetType() != AnimationType::Roll) &&
             (body_animation.GetType() != AnimationType::RollBack) &&
             (body_animation.GetType() != AnimationType::ReloadBow) &&
             (body_animation.GetType() != AnimationType::Cigar) &&
             (body_animation.GetType() != AnimationType::Match) &&
             (body_animation.GetType() != AnimationType::Smoke) &&
             (body_animation.GetType() != AnimationType::Wipe) &&
             (body_animation.GetType() != AnimationType::TakeOff) &&
             (body_animation.GetType() != AnimationType::Groin) &&
             (body_animation.GetType() != AnimationType::Piss) &&
             (body_animation.GetType() != AnimationType::Mercy) &&
             (body_animation.GetType() != AnimationType::Mercy2) &&
             (body_animation.GetType() != AnimationType::Victory) &&
             (body_animation.GetType() != AnimationType::Own) &&
             (body_animation.GetType() != AnimationType::Reload) &&
             (body_animation.GetType() != AnimationType::Prone) &&
             (body_animation.GetType() != AnimationType::GetUp) &&
             (body_animation.GetType() != AnimationType::ProneMove) &&
             (body_animation.GetType() != AnimationType::Melee)) ||
            ((body_animation.GetFrame() == body_animation.GetFramesCount()) &&
             (body_animation.GetType() != AnimationType::Prone))) {
            if (position != POS_PRONE) {
                if (position == POS_STAND) {
                    BodyApplyAnimation(AnimationType::Stand, 1);
                }

                if (position == POS_CROUCH) {
                    if (collider_distance < 255) {
                        if (body_animation.GetType() == AnimationType::HandsUpRecoil) {
                            BodyApplyAnimation(AnimationType::HandsUpAim, 11);
                        } else {
                            BodyApplyAnimation(AnimationType::HandsUpAim, 1);
                        }
                    } else {
                        if (body_animation.GetType() == AnimationType::AimRecoil) {
                            BodyApplyAnimation(AnimationType::Aim, 6);
                        } else {
                            BodyApplyAnimation(AnimationType::Aim, 1);
                        }
                    }
                }
            } else {
                BodyApplyAnimation(AnimationType::Prone, 26);
            }
        }

        if ((legs_animation.GetType() == AnimationType::Crouch) ||
            (legs_animation.GetType() == AnimationType::CrouchRun) ||
            (legs_animation.GetType() == AnimationType::CrouchRunBack)) {
            position = POS_CROUCH;
        } else {
            position = POS_STAND;
        }
        if ((legs_animation.GetType() == AnimationType::Prone) ||
            (legs_animation.GetType() == AnimationType::ProneMove)) {
            position = POS_PRONE;
        }
    }
}

void Soldier::Update(State& state, std::vector<BulletParams>& bullet_emitter)
{
    const Map& map = state.map;
    float body_y = 0.0f;
    float arm_s;

    particle.Euler();
    UpdateControl(state, bullet_emitter);

    skeleton->SetOldPos(21, skeleton->GetPos(21));
    skeleton->SetOldPos(23, skeleton->GetPos(23));
    // skeleton->SetOldPos(25, skeleton->GetPos(25));
    skeleton->SetPos(21, skeleton->GetPos(9));
    skeleton->SetPos(23, skeleton->GetPos(12));
    // skeleton->SetPos(25, skeleton->GetPos(5));

    if (!dead_meat) {
        skeleton->SetPos(21, skeleton->GetPos(21) + particle.velocity_);
        skeleton->SetPos(23, skeleton->GetPos(23) + particle.velocity_);
        // skeleton->SetPos(25, skeleton.GetPos(25) + particle.velocity);
    }
    switch (position) {
        case POS_STAND:
            body_y = 8.0F;
            break;
        case POS_CROUCH:
            body_y = 9.0F;
            break;
        case POS_PRONE: {
            if (body_animation.GetType() == AnimationType::Prone) {
                if (body_animation.GetFrame() > 9) {
                    body_y = -2.0F;
                } else {
                    body_y = 14.0F - (float)body_animation.GetFrame();
                }
            } else {
                body_y = 9.0F;
            }

            if (body_animation.GetType() == AnimationType::ProneMove) {
                body_y = 0.0F;
            }
        }
    }
    if (body_animation.GetType() == AnimationType::GetUp) {
        if (body_animation.GetFrame() > 18) {
            body_y = 8.0f;
        } else {
            body_y = 4.0f;
        }
    }

    if (control.mouse_aim_x >= particle.position.x) {
        direction = 1;
    } else {
        direction = -1;
    }

    for (int i = 1; i < 21; i++) {
        if (skeleton->GetActive(i) && !dead_meat) {
            glm::vec2 pos = glm::vec2(0.0f, 0.0f);
            skeleton->SetOldPos(i, skeleton->GetPos(i));

            if (!half_dead && ((i >= 1 && i <= 6) || (i == 17) || (i == 18))) {
                glm::vec2 anim_pos = legs_animation.GetPosition(i);
                pos.x = particle.position.x + anim_pos.x * (float)direction;
                pos.y = particle.position.y + anim_pos.y;
            }

            if ((i >= 7) && (i <= 16) || (i == 19) || (i == 20)) {
                auto anim_pos = body_animation.GetPosition(i);
                pos.x = particle.position.x + anim_pos.x * (float)direction;
                pos.y = particle.position.y + anim_pos.y;

                if (half_dead) {
                    pos.y += 9.0f;
                } else {
                    pos.y += skeleton->GetPos(6).y - (particle.position.y - body_y);
                }
            }

            skeleton->SetPos(i, pos);
        }
    }

    glm::vec2 aim(control.mouse_aim_x, control.mouse_aim_y);

    if (!dead_meat) {
        auto pos = skeleton->GetPos(9);
        auto r_norm = 0.1f * Calc::Vec2Normalize(skeleton->GetPos(12) - aim);
        auto dir = (float)direction;

        skeleton->SetPos(12, pos + glm::vec2(-dir * r_norm.y, dir * r_norm.x));
        skeleton->SetPos(23, pos + glm::vec2(-dir * r_norm.y, dir * r_norm.x) * 50.0f);
    }

    auto not_aiming_anims = std::vector{
        AnimationType::Reload,      AnimationType::ReloadBow, AnimationType::ClipIn,
        AnimationType::ClipOut,     AnimationType::SlideBack, AnimationType::Change,
        AnimationType::ThrowWeapon, AnimationType::Punch,     AnimationType::Roll,
        AnimationType::RollBack,    AnimationType::Cigar,     AnimationType::Match,
        AnimationType::Smoke,       AnimationType::Wipe,      AnimationType::TakeOff,
        AnimationType::Groin,       AnimationType::Piss,      AnimationType::Mercy,
        AnimationType::Mercy2,      AnimationType::Victory,   AnimationType::Own,
        AnimationType::Melee,
    };

    if (body_animation.GetType() == AnimationType::Throw) {
        arm_s = -5.00f;
    } else {
        arm_s = -7.00f;
    }

    if (!body_animation.IsAny(not_aiming_anims)) {
        auto r_norm = arm_s * Calc::Vec2Normalize(skeleton->GetPos(15) - aim);
        skeleton->SetPos(15, skeleton->GetPos(16) + r_norm);
    }
    if (body_animation.GetType() == AnimationType::Throw) {
        arm_s = -6.00f;
    } else {
        arm_s = -8.00f;
    }

    if (!body_animation.IsAny(not_aiming_anims)) {
        auto r_norm = arm_s * Calc::Vec2Normalize(skeleton->GetPos(19) - aim);
        skeleton->SetPos(19, skeleton->GetPos(16) - glm::vec2(0.0f, 4.0f) + r_norm);
    }

    for (int i = 1; i <= 20; i++) {
        if ((dead_meat || half_dead) && (i < 17) && (i != 7) && (i != 8)) {
            auto xy = particle.position;
            on_ground = CheckSkeletonMapCollision(map, i, xy.x, xy.y, state);
            std::cout << "ok" << std::endl;
        }
    }

    if (!dead_meat) {
        body_animation.DoAnimation();
        legs_animation.DoAnimation();

        on_ground = false;

        auto xy = particle.position;

        CheckMapCollision(map, xy.x - 3.5, xy.y - 12.0, 1, state);

        xy = particle.position;

        CheckMapCollision(map, xy.x + 3.5, xy.y - 12.0, 1, state);

        body_y = 0.0;
        arm_s = 0.0;

        // Walking either left or right (though only one can be active at once)
        if (control.left ^ control.right) {
            if (control.left ^ (direction == 1)) {
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

        xy = particle.position;
        on_ground = CheckMapCollision(map, xy.x + 2.0, xy.y + 2.0 - body_y, 0, state);

        xy = particle.position;
        on_ground |= CheckMapCollision(map, xy.x - 2.0, xy.y + 2.0 - arm_s, 0, state);

        xy = particle.position;
        auto grounded = on_ground;
        on_ground_for_law = CheckRadiusMapCollision(map, xy.x, xy.y - 1.0, grounded, state);

        xy = particle.position;
        grounded = on_ground || on_ground_for_law;
        on_ground |= CheckMapVerticesCollision(map, xy.x, xy.y, 3.0, grounded, state);

        if (!(on_ground ^ on_ground_last_frame)) {
            on_ground_permanent = on_ground;
        }

        on_ground_last_frame = on_ground;

        if ((jets_count < map.GetJetCount()) && !(control.jets)) {
            // if self.on_ground
            /* (MainTickCounter mod 2 = 0) */
            {
                jets_count += 1;
            }
        }
        alpha = 255;

        skeleton->DoVerletTimestepFor(22, 29);
        skeleton->DoVerletTimestepFor(24, 30);
    }

    if (dead_meat) {
        skeleton->DoVerletTimestep();
        particle.position = skeleton->GetPos(12);
        // CheckSkeletonOutOfBounds;
    }

    if (particle.velocity_.x > MAX_VELOCITY) {
        particle.velocity_.x = MAX_VELOCITY;
    }
    if (particle.velocity_.x < -MAX_VELOCITY) {
        particle.velocity_.x = -MAX_VELOCITY;
    }
    if (particle.velocity_.y > MAX_VELOCITY) {
        particle.velocity_.y = MAX_VELOCITY;
    }
    if (particle.velocity_.y < -MAX_VELOCITY) {
        particle.velocity_.y = MAX_VELOCITY;
    }
}

bool Soldier::CheckMapCollision(const Map& map, float x, float y, int area, State& state)
{
    auto pos = glm::vec2(x, y) + particle.velocity_;
    auto rx = (int)((pos.x / (float)map.GetSectorsSize())) + 25;
    auto ry = (int)((pos.y / (float)map.GetSectorsSize())) + 25;

    // if ((rx > 0) && (rx < map.GetSectorsCount() + 25) && (ry > 0) && (ry <
    // map.GetSectorsCount()
    // + 25)) {
    //     for (int j = 0; j < map.GetSector(rx, ry).polygonsCount; j++) {
    if (true) { // TODO: sektory nie działają
        for (int j = 0; j < map.GetPolygons().size(); j++) {
            // auto poly = map.GetSector(rx, ry).polygons[j] - 1;
            int poly = j;
            auto polytype = map.GetPolygons()[poly].polygon_type;

            if ((polytype != PMSPolygonType::NoCollide) &&
                (polytype != PMSPolygonType::OnlyBulletsCollide)) {
                auto polygons = map.GetPolygons()[poly];

                if (map.PointInPoly(pos, polygons)) {
                    HandleSpecialPolytypes(map, polytype, pos);

                    auto dist = 0.0f;
                    auto k = 0;

                    auto perp = map.ClosestPerpendicular(poly, pos, &dist, &k);

                    auto step = perp;

                    perp = Calc::Vec2Normalize(perp);
                    perp *= dist;
                    dist = Calc::Vec2Length(particle.velocity_);

                    if (Calc::Vec2Length(perp) > dist) {
                        perp = Calc::Vec2Normalize(perp);
                        perp *= dist;
                    }
                    if ((area == 0) || ((area == 1) && ((particle.velocity_.y < 0.0) ||
                                                        (particle.velocity_.x > SLIDELIMIT) ||
                                                        (particle.velocity_.x < -SLIDELIMIT)))) {
                        particle.old_position = particle.position;
                        particle.position -= perp;
                        if (map.GetPolygons()[poly].polygon_type == PMSPolygonType::Bouncy) {
                            perp = Calc::Vec2Normalize(perp);
                            perp *= map.GetPolygons()[poly].bounciness * dist;
                        }
                        particle.velocity_ -= perp;
                    }

                    if (area == 0) {
                        if ((legs_animation.GetType() == AnimationType::Stand) ||
                            (legs_animation.GetType() == AnimationType::Crouch) ||
                            (legs_animation.GetType() == AnimationType::Prone) ||
                            (legs_animation.GetType() == AnimationType::ProneMove) ||
                            (legs_animation.GetType() == AnimationType::GetUp) ||
                            (legs_animation.GetType() == AnimationType::Fall) ||
                            (legs_animation.GetType() == AnimationType::Mercy) ||
                            (legs_animation.GetType() == AnimationType::Mercy2) ||
                            (legs_animation.GetType() == AnimationType::Own)) {
                            if ((particle.velocity_.x < SLIDELIMIT) &&
                                (particle.velocity_.x > -SLIDELIMIT) && (step.y > SLIDELIMIT)) {
                                particle.position = particle.old_position;
                                glm::vec2 particle_force = particle.GetForce();
                                particle_force.y -= GRAV;
                                particle.SetForce(particle_force);
                            }

                            if ((step.y > SLIDELIMIT) && (polytype != PMSPolygonType::Ice) &&
                                (polytype != PMSPolygonType::Bouncy)) {
                                if ((legs_animation.GetType() == AnimationType::Stand) ||
                                    (legs_animation.GetType() == AnimationType::Fall) ||
                                    (legs_animation.GetType() == AnimationType::Crouch)) {
                                    particle.velocity_.x *= STANDSURFACECOEFX;
                                    particle.velocity_.y *= STANDSURFACECOEFY;

                                    glm::vec2 particle_force = particle.GetForce();
                                    particle_force.x -= particle.velocity_.x;
                                    particle.SetForce(particle_force);
                                } else if (legs_animation.GetType() == AnimationType::Prone) {
                                    if (legs_animation.GetFrame() > 24) {
                                        if (!(control.down && (control.left || control.right))) {
                                            particle.velocity_.x *= STANDSURFACECOEFX;
                                            particle.velocity_.y *= STANDSURFACECOEFY;

                                            glm::vec2 particle_force = particle.GetForce();
                                            particle_force.x -= particle.velocity_.x;
                                            particle.SetForce(particle_force);
                                        }
                                    } else {
                                        particle.velocity_.x *= SURFACECOEFX;
                                        particle.velocity_.y *= SURFACECOEFY;
                                    }
                                } else if (legs_animation.GetType() == AnimationType::GetUp) {
                                    particle.velocity_.x *= SURFACECOEFX;
                                    particle.velocity_.y *= SURFACECOEFY;
                                } else if (legs_animation.GetType() == AnimationType::ProneMove) {
                                    particle.velocity_.x *= STANDSURFACECOEFX;
                                    particle.velocity_.y *= STANDSURFACECOEFY;
                                }
                            }
                        } else if ((legs_animation.GetType() == AnimationType::CrouchRun) ||
                                   (legs_animation.GetType() == AnimationType::CrouchRunBack)) {
                            particle.velocity_.x *= CROUCHMOVESURFACECOEFX;
                            particle.velocity_.y *= CROUCHMOVESURFACECOEFY;
                        } else {
                            particle.velocity_.x *= SURFACECOEFX;
                            particle.velocity_.y *= SURFACECOEFY;
                        }
                    }

                    // AddCollidingPoly(state,
                    //  poly); // TODO: remove later, this is only a debug
                    return true;
                }
            }
        }
    }

    return false;
}

bool Soldier::CheckMapVerticesCollision(const Map& map,
                                        float x,
                                        float y,
                                        float r,
                                        bool has_collided,
                                        State& state)
{
    auto pos = glm::vec2(x, y) + particle.velocity_;
    auto rx = ((int)(pos.x / (float)map.GetSectorsSize())) + 25;
    auto ry = ((int)(pos.y / (float)map.GetSectorsSize())) + 25;

    // TODO: sektory nie działają
    if (true) { // TODO: sektory nie działają
        for (int j = 0; j < map.GetPolygons().size(); j++) {
            // auto poly = map.GetSector(rx, ry).polygons[j] - 1;
            int poly = j;
            // if ((rx > 0) && (rx < map.GetSectorsCount() + 25) && (ry > 0) && (ry <
            // map.GetSectorsCount() + 25)) {
            //     for (int j = 0; j < map.GetSector(rx, ry).polygonsCount; j++) {
            //         auto poly = map.GetSector(rx, ry).polygons[j] - 1;
            PMSPolygonType polytype = map.GetPolygons()[poly].polygon_type;

            if (polytype != PMSPolygonType::NoCollide &&
                polytype != PMSPolygonType::OnlyBulletsCollide) {
                for (int i = 0; i < 3; i++) {
                    auto vert = glm::vec2(map.GetPolygons()[poly].vertices[i].x,
                                          map.GetPolygons()[poly].vertices[i].y);

                    auto dist = Calc::Distance(vert, pos);
                    if (dist < r) {
                        if (!has_collided) {
                            HandleSpecialPolytypes(map, polytype, pos);
                        }
                        auto dir = pos - vert;
                        dir = Calc::Vec2Normalize(dir);
                        particle.position += dir;
                        // AddCollidingPoly(state,
                        //  poly); // TODO: remove later, this is only a debug
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool Soldier::CheckRadiusMapCollision(const Map& map,
                                      float x,
                                      float y,
                                      bool has_collided,
                                      State& state)
{
    auto s_pos = glm::vec2(x, y - 3.0f);

    auto det_acc = (int)Calc::Vec2Length(particle.velocity_);
    if (det_acc == 0) {
        det_acc = 1;
    }

    auto step = particle.velocity_ * (1.0f / (float)det_acc);

    for (int _z = 0; _z < det_acc; _z++) {
        s_pos += step;

        auto rx = ((int)(s_pos.x / (float)map.GetSectorsSize())) + 25;
        auto ry = ((int)(s_pos.y / (float)map.GetSectorsSize())) + 25;

        if (true) { // TODO: sektory nie działają
            for (int j = 0; j < map.GetPolygons().size(); j++) {
                // auto poly = map.GetSector(rx, ry).polygons[j] - 1;
                int poly = j;
                // if ((rx > 0) && (rx < map.GetSectorsCount() + 25) && (ry > 0) && (ry
                // < map.GetSectorsCount() + 25)) {
                //     for (int j = 0; j < map.GetSector(rx, ry).polygonsCount; j++) {
                //         auto poly = map.GetSector(rx, ry).polygons[j] - 1;
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
                                HandleSpecialPolytypes(map, polytype, pos);
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

                            particle.position = particle.old_position;
                            particle.velocity_ = particle.GetForce() - perp;

                            // AddCollidingPoly(state,
                            //  poly); // TODO: remove later, this is only a debug
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool Soldier::CheckSkeletonMapCollision(const Map& map,
                                        unsigned int i,
                                        float x,
                                        float y,
                                        State& state)
{
    auto result = false;
    auto pos = glm::vec2(x - 1.0f, y + 4.0f);
    auto rx = ((int)(pos.x / (float)map.GetSectorsSize())) + 25;
    auto ry = ((int)(pos.y / (float)map.GetSectorsSize())) + 25;

    if (true) { // TODO: sektory nie działają
        for (int j = 0; j < map.GetPolygons().size(); j++) {
            // auto poly = map.GetSector(rx, ry).polygons[j] - 1;
            int poly = j;
            // if ((rx > 0) && (rx < map.GetSectorsCount() + 25) && (ry > 0) && (ry <
            // map.GetSectorsCount() + 25)) {
            //     for (int j = 0; j < map.GetSector(rx, ry).polygonsCount; j++) {
            //         auto poly = map.GetSector(rx, ry).polygons[j] - 1;

            if (map.PointInPolyEdges(pos.x, pos.y, poly)) {
                auto dist = 0.0f;
                auto b = 0;
                auto perp = map.ClosestPerpendicular(poly, pos, &dist, &b);
                perp = Calc::Vec2Normalize(perp);
                perp *= dist;

                skeleton->SetPos(i, skeleton->GetOldPos(i) - perp);
                // AddCollidingPoly(state,
                //                  poly); // TODO: remove later, this is only a debug
                result = true;
            }
        }
    }

    if (result) {
        auto pos = glm::vec2(x, y + 1.0);
        auto rx = ((int)(pos.x / (float)map.GetSectorsSize())) + 25;
        auto ry = ((int)(pos.y / (float)map.GetSectorsSize())) + 25;

        if (true) { // TODO: sektory nie działają
            for (int j = 0; j < map.GetPolygons().size(); j++) {
                // auto poly = map.GetSector(rx, ry).polygons[j] - 1;
                int poly = j;
                // if ((rx > 0) && (rx < map.GetSectorsCount() + 25) && (ry > 0) && (ry
                // < map.GetSectorsCount() + 25)) {
                //     for (int j = 0; j < map.GetSector(rx, ry).polygonsCount; j++) {
                //         auto poly = map.GetSector(rx, ry).polygons[j] - 1;
                // if (Map.PolyType[poly] <> POLY_TYPE_DOESNT) and (Map.PolyType[poly]
                // <> POLY_TYPE_ONLY_BULLETS) then
                if (map.PointInPolyEdges(pos.x, pos.y, poly)) {
                    auto dist = 0.0f;
                    auto b = 0;
                    auto perp = map.ClosestPerpendicular(poly, pos, &dist, &b);
                    perp = Calc::Vec2Normalize(perp);
                    perp *= dist;

                    skeleton->SetPos(i, skeleton->GetOldPos(i) - perp);
                    // AddCollidingPoly(state,
                    //  poly); // TODO: remove later, this is only a debug
                    result = true;
                }
            }
        }
    }

    return result;
}

void Soldier::Fire(std::vector<BulletParams>& bullet_emitter)
{
    auto weapon = GetPrimaryWeapon();

    glm::vec2 dir;
    if (weapon.GetWeaponParameters().bullet_style == BulletType::Blade ||
        body_animation.GetType() == AnimationType::Mercy ||
        body_animation.GetType() == AnimationType::Mercy2) {
        dir = Calc::Vec2Normalize(skeleton->GetPos(15) - skeleton->GetPos(16));
    } else {
        auto aim_x = (float)control.mouse_aim_x;
        auto aim_y = (float)control.mouse_aim_y;
        dir = Calc::Vec2Normalize(glm::vec2(aim_x, aim_y) - skeleton->GetPos(15));
    };

    auto pos = skeleton->GetPos(15) + dir * 4.0F - glm::vec2(0.0, 2.0);
    auto bullet_velocity = dir * weapon.GetWeaponParameters().speed;
    auto inherited_velocity = particle.velocity_ * weapon.GetWeaponParameters().inherited_velocity;

    BulletParams params{
        weapon.GetWeaponParameters().bullet_style,
        weapon.GetWeaponParameters().kind,
        pos,
        bullet_velocity + inherited_velocity,
        (std::int16_t)weapon.GetWeaponParameters().timeout,
        weapon.GetWeaponParameters().hit_multiply,
        TeamType::None,
    };

    switch (weapon.GetWeaponParameters().kind) {
        case WeaponType::DesertEagles: {
            bullet_emitter.push_back(params);

            auto signx = dir.x > 0.0F ? 1.0F : (dir.x < 0.0F ? -1.0F : 0.0F);
            auto signy = dir.x > 0.0F ? 1.0F : (dir.x < 0.0F ? -1.0F : 0.0F);

            params.position += glm::vec2(-signx * dir.y, signy * dir.x) * 3.0F;
            bullet_emitter.push_back(params);
            break;
        }
        case WeaponType::Spas12:
        case WeaponType::Flamer:
        case WeaponType::NoWeapon:
        case WeaponType::Knife:
        case WeaponType::Chainsaw:
        case WeaponType::LAW:
            break;
        default: {
            bullet_emitter.push_back(params);
        }
    };
}
} // namespace Soldat
