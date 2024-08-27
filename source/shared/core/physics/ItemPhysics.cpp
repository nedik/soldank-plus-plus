#include "core/physics/ItemPhysics.hpp"

#include "core/entities/Item.hpp"
#include "core/entities/Soldier.hpp"
#include "core/map/Map.hpp"
#include "core/map/PMSEnums.hpp"
#include "core/math/Calc.hpp"
#include "core/state/State.hpp"
#include "core/physics/PhysicsEvents.hpp"
#include "core/physics/Constants.hpp"
#include "core/types/BulletType.hpp"

#include <chrono>
#include <cmath>
#include <spdlog/spdlog.h>
#include <xtimec.h>

namespace Soldank::ItemPhysics
{
bool IsItemFlag(const Item& item)
{
    return item.style == ItemType::AlphaFlag || item.style == ItemType::BravoFlag ||
           item.style == ItemType::PointmatchFlag;
}

void Update(State& state, Item& item, const PhysicsEvents& physics_events)
{
    bool was_static = item.static_type;

    if (!item.static_type) {
        bool collided = false;
        bool collided2 = false;

        // BGState.BackgroundTestPrepare();

        for (int i = 1; i <= 4; ++i) {
            if (item.skeleton->GetActive(i) &&
                (item.holding_sprite == 0 || (item.holding_sprite > 0 && i == 1))) {

                if (IsItemFlag(item)) {
                    if (i == 1) {
                        if (CheckMapCollision(item,
                                              state.map,
                                              item.skeleton->GetPos(i).x - 10,
                                              item.skeleton->GetPos(i).y - 8,
                                              i,
                                              state) ||
                            CheckMapCollision(item,
                                              state.map,
                                              item.skeleton->GetPos(i).x + 10,
                                              item.skeleton->GetPos(i).y - 8,
                                              i,
                                              state) ||
                            CheckMapCollision(item,
                                              state.map,
                                              item.skeleton->GetPos(i).x - 10,
                                              item.skeleton->GetPos(i).y,
                                              i,
                                              state) ||
                            CheckMapCollision(item,
                                              state.map,
                                              item.skeleton->GetPos(i).x + 10,
                                              item.skeleton->GetPos(i).y,
                                              i,
                                              state)) {
                            if (collided) {
                                collided2 = true;
                            }

                            collided = true;
                            if (collided) {
                                auto force = item.skeleton->GetForce(2);
                                force.y +=
                                  PhysicsConstants::FLAG_STAND_FORCEUP * PhysicsConstants::GRAV;
                                item.skeleton->SetForce(2, force);
                            }
                        }
                    } else {
                        if (CheckMapCollision(item,
                                              state.map,
                                              item.skeleton->GetPos(i).x,
                                              item.skeleton->GetPos(i).y,
                                              i,
                                              state)) {
                            if (collided) {
                                collided2 = true;
                            }
                            collided = true;
                        }
                    }
                } else {
                    if (CheckMapCollision(item,
                                          state.map,
                                          item.skeleton->GetPos(i).x,
                                          item.skeleton->GetPos(i).y,
                                          i,
                                          state)) {
                        if (collided) {
                            collided2 = true;
                        }
                        collided = true;
                    }
                }
            }
        }

        // BGState.BackgroundTestReset();

        item.skeleton->DoVerletTimestep();

        if (item.style == ItemType::M2 && item.time_out < 0) {
            item.skeleton->SetPos(2, item.skeleton->GetOldPos(2));
            item.skeleton->SetPos(3, item.skeleton->GetOldPos(3));
        }

        auto a = item.skeleton->GetPos(1) - item.skeleton->GetOldPos(1);
        auto b = item.skeleton->GetPos(2) - item.skeleton->GetOldPos(2);
        if (item.style != ItemType::M2) {
            if (collided && collided2) {
                if ((Calc::Vec2Length(a) + Calc::Vec2Length(b)) / 2 <
                    PhysicsConstants::MINMOVEDELTA) {
                    item.static_type = true;
                }
            }
        }
    }

    if (item.style != ItemType::M2) {
        int colliding_player_id = CheckSoldierCollision(item, state);
        if (colliding_player_id >= 0) {
            for (auto it = state.soldiers.begin(); it != state.soldiers.end(); ++it) {
                if (colliding_player_id == it->id) {
                    physics_events.soldier_collides_with_item.Notify(*it, item);
                }
            }
        }
    }

    --item.time_out;
    if (item.time_out < -1000) {
        item.time_out = -1000;
    }

    if (item.time_out == 0) {
        switch (item.style) {
            case ItemType::AlphaFlag:
            case ItemType::BravoFlag:
            case ItemType::PointmatchFlag:
            case ItemType::Bow:
                // begin
                // {$IFDEF SERVER}
                // if HoldingSprite > 0 then
                //     TimeOut := FLAG_TIMEOUT else
                //     Respawn;
                // {$ENDIF}
                // end;
                break;
            default:
                item.active = false;
                break;
        }
    }

    // CheckOutOfBounds;

    if ((!was_static) && item.static_type) {
        item.skeleton->SetPos(1, item.skeleton->GetOldPos(1));
        item.skeleton->SetPos(2, item.skeleton->GetOldPos(2));
        item.skeleton->SetPos(3, item.skeleton->GetOldPos(3));
        item.skeleton->SetPos(4, item.skeleton->GetOldPos(4));
    }
}

bool CheckMapCollision(Item& item, const Map& map, float x, float y, int i, State& state)
{
    glm::vec2 pos = { x, y - 1.0F }; // TODO: this looks wrong
    auto rx = ((int)std::round((pos.x / (float)map.GetSectorsSize()))) + 25;
    auto ry = ((int)std::round((pos.y / (float)map.GetSectorsSize()))) + 25;
    if ((rx > 0) && (rx < map.GetSectorsCount() + 25) && (ry > 0) &&
        (ry < map.GetSectorsCount() + 25)) {
        // TODO
        //  BGState.BackgroundTestBigPolyCenter(Pos);

        for (unsigned int j = 0; j < map.GetSector(rx, ry).polygons.size(); j++) {
            auto w = map.GetSector(rx, ry).polygons[j] - 1;

            auto teamcol = true;

            // if (Owner > 0) and (Owner < MAX_SPRITES + 1) then
            // begin
            //     teamcol := TeamCollides(w, Sprite[Owner].Player.Team, False);
            // end;

            // if (Style < OBJECT_USSOCOM) and (Map.PolyType[w] > POLY_TYPE_LAVA) and
            // (Map.PolyType[w] < POLY_TYPE_BOUNCY) then
            //     teamcol := False;

            if (teamcol &&
                (map.GetPolygons().at(w).polygon_type != PMSPolygonType::OnlyBulletsCollide) &&
                (map.GetPolygons().at(w).polygon_type != PMSPolygonType::OnlyPlayersCollide) &&
                (map.GetPolygons().at(w).polygon_type != PMSPolygonType::NoCollide) &&
                (map.GetPolygons().at(w).polygon_type != PMSPolygonType::FlaggerCollides) &&
                (map.GetPolygons().at(w).polygon_type != PMSPolygonType::NonFlaggerCollides)) {
                if (map.PointInPolyEdges(pos.x, pos.y, w)) {
                    // if BGState.BackgroundTest(w) then
                    //     Continue;

                    float d = NAN;
                    int b = 0;
                    auto perp = map.ClosestPerpendicular(w, pos, &d, &b);

                    perp = Calc::Vec2Normalize(perp);
                    perp = Calc::Vec2Scale(perp, d);

                    switch (item.style) {
                        case ItemType::AlphaFlag:
                        case ItemType::BravoFlag:
                        case ItemType::PointmatchFlag:

                        case ItemType::USSOCOM:
                        case ItemType::DesertEagles:
                        case ItemType::MP5:
                        case ItemType::Ak74:
                        case ItemType::SteyrAUG:
                        case ItemType::Spas12:
                        case ItemType::Ruger77:
                        case ItemType::M79:
                        case ItemType::Barrett:
                        case ItemType::Minimi:
                        case ItemType::Minigun:
                        case ItemType::Knife:
                        case ItemType::Chainsaw:
                        case ItemType::LAW: {
                            break;
                        }
                        case ItemType::Bow:
                        case ItemType::MedicalKit:
                        case ItemType::GrenadeKit:
                        case ItemType::FlamerKit:
                        case ItemType::PredatorKit:
                        case ItemType::VestKit:
                        case ItemType::BerserkKit:
                        case ItemType::ClusterKit: {
                            item.skeleton->SetPos(i, item.skeleton->GetOldPos(i));
                            item.skeleton->SetPos(i, item.skeleton->GetPos(i) - perp);
                            break;
                        }
                        case ItemType::Parachute:
                        case ItemType::M2:
                            break;
                    }

                    ++item.collide_count.at(i - 1);

                    return true;
                }
            }
        }
    }
    return false;
}

const int STARTHEALTH = 150; // TODO: move me somewhere else
const int MAXGRENADES = 5;   // TODO: move me somewhere else

int CheckSoldierCollision(Item& item, State& state)
{
    glm::vec2 a = item.skeleton->GetPos(1) - item.skeleton->GetPos(2);
    float k = Calc::Vec2Length(a) / 2;
    a = Calc::Vec2Normalize(a);
    a = Calc::Vec2Scale(a, -k);
    glm::vec2 pos = item.skeleton->GetPos(1) + a;

    float closestdist = 99999999.0;
    int closestplayer = -1;

    for (auto soldier_it = state.soldiers.begin(); soldier_it != state.soldiers.end();
         ++soldier_it) {
        if (soldier_it->active &&
            !soldier_it->dead_meat /* && soldier_it->team != SPECTATOR TODO*/) {
            glm::vec2 col_pos = soldier_it->particle.position;
            glm::vec2 norm = pos - col_pos;
            if (Calc::Vec2Length(norm) >= item.radius) {
                pos = item.skeleton->GetPos(1);
                col_pos = soldier_it->particle.position;
                norm = pos - col_pos;

                if (Calc::Vec2Length(norm) >= item.radius) {
                    pos = item.skeleton->GetPos(2);
                    col_pos = soldier_it->particle.position;
                    norm = pos - col_pos;
                }
            }

            float dist = Calc::Vec2Length(norm);
            if (dist < item.radius) {
                if (dist < closestdist) {
                    if (item.style != ItemType::MedicalKit || soldier_it->health != STARTHEALTH) {
                        if (item.style != ItemType::GrenadeKit ||
                            soldier_it->weapons[2].GetAmmoCount() != MAXGRENADES ||
                            soldier_it->weapons[2].GetWeaponParameters().bullet_style !=
                              BulletType::FragGrenade) {
                            if (!(IsItemFlag(
                                  item) /*&& soldier_it->cease_fire_counter > 0 TODO */)) {
                                closestdist = dist;
                                closestplayer = soldier_it->id;
                            }
                        }
                    }
                }
            }
        }
    }

    return closestplayer;
}

} // namespace Soldank::ItemPhysics
