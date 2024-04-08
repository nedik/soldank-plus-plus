#include "core/physics/BulletPhysics.hpp"

#include "core/entities/Bullet.hpp"
#include "core/map/PMSEnums.hpp"
#include "core/map/PMSStructs.hpp"
#include "core/math/Calc.hpp"
#include "core/types/BulletType.hpp"
#include "core/types/TeamType.hpp"
#include "core/types/WeaponType.hpp"
#include "core/entities/WeaponParametersFactory.hpp"

#include "spdlog/spdlog.h"

#include <algorithm>
#include <array>

namespace Soldank
{
void BulletPhysics::UpdateBullet(const PhysicsEvents& physics_events,
                                 Bullet& bullet,
                                 const Map& map,
                                 State& state)
{
    bullet.velocity_prev = bullet.particle.velocity_;
    bullet.particle.Euler();

    auto collision = CheckMapCollision(bullet, map);
    if (collision) {
        bullet.particle.position = collision.value().first;
        bullet.active = false;
    }

    bullet.timeout_prev = bullet.timeout;
    bullet.timeout -= 1;

    if (bullet.timeout == 0) {
        bullet.active = false;
    } else if (bullet.degrade_count < 2 && bullet.timeout % 6 == 0) {
        constexpr auto EXCEPT =
          std::array{ WeaponType::Barrett, WeaponType::M79, WeaponType::Knife, WeaponType::LAW };

        if (!std::ranges::contains(EXCEPT, bullet.weapon)) {
            // let dist2 = (self.particle.pos - self.initial_pos).magnitude2();
            auto delta = bullet.particle.position - bullet.initial_position;
            auto dist2 = delta.x * delta.x + delta.y * delta.y;
            auto degrade_dists2 = std::array{ 500.0F * 500.0F, 900.0F * 900.0F };

            if (dist2 > degrade_dists2.at(bullet.degrade_count)) {
                bullet.hit_multiply_prev = bullet.hit_multiply;
                bullet.hit_multiply *= 0.5;
                bullet.degrade_count += 1;
            }
        }
    }

    float x = bullet.particle.position.x;
    float y = bullet.particle.position.y;
    if (std::max(std::abs(x), std::abs(y)) >
        (float)(map.GetSectorsCount() * map.GetSectorsSize() - 10)) {
        bullet.active = false;
    }

    auto soldier_collision = CheckSoldierCollision(physics_events, bullet, map, state, -1.0F);
    if (soldier_collision.has_value()) {
        bullet.active = false;
        spdlog::debug("soldier hit");
    }
}

std::optional<std::pair<glm::vec2, unsigned int>> BulletPhysics::CheckMapCollision(Bullet& bullet,
                                                                                   const Map& map)
{
    auto a = bullet.particle.old_position;
    auto b = bullet.particle.position;

    auto delta = b - a;
    float steps = std::ceil(glm::length(delta) / 2.5F);

    for (float i = 0; i <= steps; i++) {
        auto xy = Calc::Lerp(a, b, i / steps);

        auto rx = ((int)std::round((xy.x / (float)map.GetSectorsSize()))) + 25;
        auto ry = ((int)std::round((xy.y / (float)map.GetSectorsSize()))) + 25;

        if ((rx > 0) && (rx < map.GetSectorsCount() + 25) && (ry > 0) &&
            (ry < map.GetSectorsCount() + 25)) {
            for (int j = 0; j < map.GetSector(rx, ry).polygons.size(); j++) {
                auto poly_id = map.GetSector(rx, ry).polygons[j] - 1;
                const PMSPolygon& poly = map.GetPolygons()[poly_id];
                if (CollidesWithPoly(poly, bullet.team) &&
                    Map::PointInPoly(glm::vec2(xy.x, xy.y), poly)) {
                    return std::make_pair(xy, poly_id);
                }
            }
        }
    }

    return std::nullopt;
}

bool BulletPhysics::CollidesWithPoly(const PMSPolygon& poly, TeamType team)
{
    switch (poly.polygon_type) {
        case PMSPolygonType::AlphaBullets:
            return team == TeamType::Alpha;
        case PMSPolygonType::BravoBullets:
            return team == TeamType::Bravo;
        case PMSPolygonType::CharlieBullets:
            return team == TeamType::Charlie;
        case PMSPolygonType::DeltaBullets:
            return team == TeamType::Delta;
        case PMSPolygonType::AlphaPlayers:
        case PMSPolygonType::BravoPlayers:
        case PMSPolygonType::CharliePlayers:
        case PMSPolygonType::DeltaPlayers:
        case PMSPolygonType::OnlyPlayersCollide:
        case PMSPolygonType::NoCollide:
        case PMSPolygonType::FlaggerCollides:
        case PMSPolygonType::NonFlaggerCollides:
            // TODO: missing polygon types
            // case PMSPolygonType::ptBACKGROUND:
            // case PMSPolygonType::ptBACKGROUND_TRANSITION:
            return false;
        default:
            return true;
    }
}

std::optional<glm::vec2> BulletPhysics::CheckSoldierCollision(const PhysicsEvents& physics_events,
                                                              Bullet& bullet,
                                                              const Map& map,
                                                              State& state,
                                                              float lasthitdist)
{
    // TODO: can't throw knife (with short hold) because it immediately collides with the owner
    const auto body_parts_priority = std::array{ 12, 11, 10, 6, 5, 4, 3 };

    std::optional<glm::vec2> result = std::nullopt;

    int arrow_resist = 280; // TODO: this is a constant
    if (bullet.style == BulletType::Arrow && bullet.timeout <= arrow_resist) {
        return result;
    }

    glm::vec2 a;
    glm::vec2 pos;
    glm::vec2 bullet_velocity = bullet.particle.GetVelocity();

    // Iterate through sprites
    if (bullet.style != BulletType::ClusterGrenade) {
        // TODO: filter soldiers by distance
        for (auto& soldier : state.soldiers) {
            auto col = GetSoldierCollisionPoint(soldier);

            int where = 0;
            int r = 0;
            const int part_radius = 7; // TODO: const
            if (bullet.style != BulletType::FragGrenade) {
                r = part_radius;
            } else {
                r = part_radius + 1;
            }

            glm::vec2 start_point;
            glm::vec2 end_point;
            // Pre-calculate some points if it's a melee weapon
            if (bullet.style == BulletType::Fist || bullet.style == BulletType::Blade) {
                pos = bullet.particle.position;

                // TODO: implement this
                // glm::vec2 buttstock_position_offset =
                // state.soldiers[bullet.owner].GetHandsAimDirection();
            } else {
                start_point = bullet.particle.position;
                end_point = bullet.particle.position + bullet.particle.GetVelocity();
            }

            // Check for collision with the body parts
            float min_dist = 9999999; // TODO: max float

            for (auto body_part_id : body_parts_priority) {
                auto body_part_offset =
                  soldier.skeleton->GetPos(body_part_id) - soldier.particle.position;
                auto col_pos = col + body_part_offset;

                // TODO: check opensoldat code
                if (bullet.style != BulletType::Fist && bullet.style != BulletType::Blade) {
                    col_pos.x -= 2.0F;
                }

                auto p = Calc::LineCircleCollision(start_point, end_point, col_pos, r);
                if (p.has_value()) {
                    pos = *p;
                    if (Calc::LineCircleCollision(start_point, end_point, col_pos, r)) {
                        auto dist = Calc::SquareDistance(start_point, pos);

                        if (dist < min_dist) {
                            where = body_part_id;
                            min_dist = dist;
                        }
                    }
                }
            }

            // temporary:
            // if (where != 0) {
            //     return glm::vec2{ 0, 0 };
            // }

            if ((bullet.style != BulletType::Fist && bullet.style != BulletType::Blade) ||
                soldier.id != bullet.owner_id) {

                if (where != 0) {
                    // order collision
                    if (lasthitdist > -1) {
                        a = pos - bullet.particle.old_position;
                        auto dist = Calc::Vec2Length(a);

                        if (dist > lasthitdist) {
                            break;
                        }
                    }

                    // soldier.brain.pissed_off = bullet.owner_id

                    auto norm = pos - soldier.skeleton->GetPos(where);
                    norm = Calc::Vec2Scale(norm, 1.3);
                    norm.y = -norm.y;

                    result = pos;

                    // NoCollision:
                    //     = Guns[WeaponNumToIndex(OwnerWeapon)].NoCollision;

                    //     if (NoCollision and WEAPON_NOCOLLISION_ENEMY<> 0)
                    //         and(Sprite[j].IsNotInSameTeam(Sprite[Owner])) then Continue;

                    //     if (NoCollision and WEAPON_NOCOLLISION_TEAM<> 0)
                    //         and(Sprite[j].IsInSameTeam(Sprite[Owner])) and (j<> Owner) then
                    //         Continue;

                    //     if (NoCollision and WEAPON_NOCOLLISION_SELF<> 0)
                    //         and(Owner = j) then Continue;

                    if (!soldier.dead_meat) {
                        if (bullet.style != BulletType::FragGrenade &&
                            bullet.style != BulletType::Flame &&
                            bullet.style != BulletType::Arrow) {
                            auto bullet_push = Calc::Vec2Scale(bullet_velocity, bullet.push);
                            //         PushTick := Sprite[j].Player.PingTicks div 2 +
                            //   OwnerPingTick + 1;
                            // if PushTick > MAX_PUSHTICK then
                            //   PushTick := MAX_PUSHTICK;
                            // Sprite[j].NextPush[PushTick] := Vec2Add(
                            //   Sprite[j].NextPush[PushTick], BulletPush);
                        }
                    }

                    /*if (Sprite[j].CeaseFireCounter < 0)*/ {
                        switch (bullet.style) {
                            case BulletType::Bullet:
                            case BulletType::GaugeBullet:
                            case BulletType::Fist:
                            case BulletType::Blade:
                            case BulletType::M2Bullet: {
                                // TODO: Blood spark
                                // TODO: Shake screen
                                // TODO: Puff
                                // TODO: Shread clothes
                                // TODO: play hit sound
                                auto hitbox_modifier = 1.0F;
                                // TODO: take hitbox_modifier
                                if (where <= 4) {
                                    hitbox_modifier =
                                      WeaponParametersFactory::GetParameters(
                                        bullet.weapon, false /* TODO realistic or not*/)
                                        .modifier_legs;
                                } else if (where <= 11) {
                                    hitbox_modifier =
                                      WeaponParametersFactory::GetParameters(
                                        bullet.weapon, false /* TODO realistic or not*/)
                                        .modifier_chest;
                                } else {
                                    hitbox_modifier =
                                      WeaponParametersFactory::GetParameters(
                                        bullet.weapon, false /* TODO realistic or not*/)
                                        .modifier_head;
                                }

                                auto speed = Calc::Vec2Length(bullet_velocity);
                                auto was_dead = soldier.dead_meat;
                                // TODO: hit soldier
                                auto hit_multiply =
                                  WeaponParametersFactory::GetParameters(
                                    bullet.weapon, false /* TODO realistic or not*/)
                                    .hit_multiply;
                                auto damage = speed * hit_multiply * hitbox_modifier;
                                physics_events.soldier_hit_by_bullet.Notify(soldier, damage);

                                // TODO: hit spray
                                // TODO: drop weapon if hit from close distance
                                // if (bullet.style == BulletType::Fist) {
                                //     if (soldier.IsSolo() || (soldier.IsNotSolo() &&
                                //     soldier.IsNotInSameTeam(state.soldiers[owner_id]))) {
                                //         if (soldier.weapon != WeaponType::Bow && soldier.weapon
                                //         != WeaponType::FlameBow) {
                                //             soldier.ApplyBodyAnimation(ThrowWeapon, 11);
                                //         }
                                //     }
                                // }

                                if (was_dead) {
                                    bullet.particle.SetVelocity(
                                      Calc::Vec2Scale(bullet_velocity, 0.9F));
                                    bullet_velocity = bullet.particle.GetVelocity();
                                    // TODO: Hit(HIT_TYPE_BODYHIT)
                                    continue;
                                }

                                if (soldier.dead_meat || speed > 23) {
                                    bullet.particle.SetVelocity(
                                      Calc::Vec2Scale(bullet_velocity, 0.75F));
                                    bullet_velocity = bullet.particle.GetVelocity();
                                    // TODO: Hit(HIT_TYPE_BODYHIT)
                                }

                                // if (speed > 5 && (speed / Guns[WeaponIndex].Speed >= 0.9)) {
                                //     bullet.particle.SetVelocity(
                                //       Calc::Vec2Scale(bullet_velocity, 0.66F));
                                //     bullet_velocity = bullet.particle.GetVelocity();
                                //     // TODO: Hit(HIT_TYPE_BODYHIT)
                                // }

                                // TODO: destroy bullet
                                bullet.active = false;
                                break;
                            }
                            case BulletType::FragGrenade: {
                                // TODO
                                break;
                            }
                            case BulletType::Arrow: {
                                // TODO
                                break;
                            }
                            case BulletType::M79Grenade:
                            case BulletType::FlameArrow:
                            case BulletType::LAWMissile: {
                                // TODO
                                break;
                            }
                            case BulletType::Flame: {
                                // TODO
                                break;
                            }
                            case BulletType::Cluster: {
                                // TODO
                                break;
                            }
                            case BulletType::ThrownKnife: {
                                // TODO
                                break;
                            }
                        }

                        // Bullet is destroyed, so exit
                        return result;
                    }
                }
            }
        }
    }

    return result;
}

glm::vec2 BulletPhysics::GetSoldierCollisionPoint(Soldier& soldier)
{
    // TODO: check in opensoldat code
    return soldier.particle.position;
}
} // namespace Soldank
