#include "SoldierRenderer.hpp"

#include "core/types/WeaponType.hpp"
#include "rendering/data/Sprites.hpp"
#include "rendering/data/Texture.hpp"
#include "rendering/renderer/Renderer.hpp"
#include "rendering/renderer/SoldierRenderer.hpp"
#include "rendering/shaders/ShaderSources.hpp"

#include "core/math/Calc.hpp"
#include "core/utility/VisitHelper.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <cmath>

namespace Soldat
{
SoldierRenderer::SoldierRenderer()
    : shader_(ShaderSources::DYNAMIC_COLOR_VERTEX_SHADER_SOURCE,
              ShaderSources::DYNAMIC_COLOR_FRAGMENT_SHADER_SOURCE)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices{ 0, 1, 2, 1, 3, 2 };

    for (unsigned int i = 0; i < Sprites::GetSoldierPartCount(); i++) {
        vertices.clear();

        const Sprites::SoldierPartData* part_data = Sprites::Get(i);
        if (part_data == nullptr) {
            vbos_.emplace_back(0, 0);
            ebos_.push_back(0);
            continue;
        }

        GenerateVertices(vertices, *part_data, false);
        unsigned int vbo_for_original = Renderer::CreateVBO(vertices, GL_STATIC_DRAW);
        std::optional<unsigned int> vbo_for_flipped = std::nullopt;
        if (part_data->IsFlippable()) {
            vertices.clear();
            GenerateVertices(vertices, *part_data, true);
            vbo_for_flipped = Renderer::CreateVBO(vertices, GL_STATIC_DRAW);
        }

        vbos_.emplace_back(vbo_for_original, vbo_for_flipped);
        ebos_.push_back(Renderer::CreateEBO(indices, GL_STATIC_DRAW));
    }
}

SoldierRenderer::~SoldierRenderer()
{
    for (unsigned int ebo : ebos_) {
        Renderer::FreeEBO(ebo);
    }
    for (auto vbo : vbos_) {
        Renderer::FreeEBO(vbo.first);
        if (vbo.second.has_value()) {
            Renderer::FreeEBO(*vbo.second);
        }
    }
}

void SoldierRenderer::GenerateVertices(std::vector<float>& vertices,
                                       const Sprites::SoldierPartData& part_data,
                                       bool flipped)
{
    float texture_width = NAN;
    float texture_height = NAN;
    float pivot_x = part_data.GetCenter().x;
    float pivot_y = (1.0F - part_data.GetCenter().y);
    if (!flipped) {
        texture_width = (float)part_data.GetTextureWidth();
        texture_height = (float)part_data.GetTextureHeight();
    } else {
        texture_width = (float)part_data.GetTextureFlippedWidth();
        texture_height = (float)part_data.GetTextureFlippedHeight();
        pivot_y = 1.0F - pivot_y;
    }
    pivot_x *= texture_width;
    pivot_y *= texture_height;

    float w0 = 0.0F - pivot_x;
    float w1 = texture_width - pivot_x;
    float h0 = 0.0F - pivot_y;
    float h1 = texture_height - pivot_y;

    // clang-format off
    vertices = {
        // position         // texture
        w0, h0, 1.0,        0.0, 0.0,
        w1, h0, 1.0,        1.0, 0.0,
        w0, h1, 1.0,        0.0, 1.0,
        w1, h1, 1.0,        1.0, 1.0,
    };
    // clang-format on
}

void SoldierRenderer::Render(glm::mat4 transform, const Soldier& soldier, double frame_percent)
{
    shader_.Use();

    for (unsigned int i = 0; i < vbos_.size(); i++) {
        const Sprites::SoldierPartData* part_data = Sprites::Get(i);
        if (part_data == nullptr) {
            continue;
        }

        bool part_base_visibility = part_data->IsVisible();
        auto part_type = Sprites::GetType(i);
        bool part_visible = std::visit(
          VisitOverload{
            [&soldier, part_base_visibility](Sprites::SoldierPartType soldier_part_type) {
                return SoldierRenderer::IsSoldierPartTypeVisible(
                  soldier_part_type, soldier, part_base_visibility);
            },
            [&soldier, part_base_visibility](Sprites::SoldierPartPrimaryWeaponType weapon_type) {
                return SoldierRenderer::IsPrimaryWeaponTypeVisible(
                  weapon_type, soldier, part_base_visibility);
            },
            [&soldier](Sprites::SoldierPartSecondaryWeaponType weapon_type) {
                return SoldierRenderer::IsSecondaryWeaponTypeVisible(weapon_type, soldier);
            },
            [&soldier](Sprites::SoldierPartTertiaryWeaponType weapon_type) {
                return SoldierRenderer::IsTertiaryWeaponTypeVisible(weapon_type, soldier);
            } },
          part_type);

        if (!part_visible) {
            continue;
        }

        unsigned int sprite_texture = part_data->GetTexture();
        glm::vec2 scale = glm::vec2(1.0, 1.0);
        unsigned int px = part_data->GetPoint().x;
        unsigned int py = part_data->GetPoint().y;
        glm::vec2 p0 = Calc::Lerp(
          soldier.skeleton->GetOldPos(px), soldier.skeleton->GetPos(px), (float)frame_percent);
        glm::vec2 p1 = Calc::Lerp(
          soldier.skeleton->GetOldPos(py), soldier.skeleton->GetPos(py), (float)frame_percent);
        float rot = Calc::Vec2Angle(p1 - p0);

        unsigned int vbo_to_use = vbos_[i].first;
        if (soldier.direction != 1) {
            if (part_data->IsFlippable()) {
                sprite_texture = part_data->GetTextureFlipped();
                vbo_to_use = *vbos_[i].second;
            } else {
                scale.y = -1.0;
            }
        }

        if (part_data->GetFlexibility() > 0.0) {
            scale.x = std::min(1.5F, glm::length(p1 - p0) / part_data->GetFlexibility());
        }

        // TODO: magic numbers, this is in mod.ini
        scale.x /= 4.5;
        scale.y /= 4.5;

        Renderer::SetupVertexArray(vbo_to_use, ebos_[i], false, true);
        glm::mat4 current_part_transform = transform;

        current_part_transform =
          glm::translate(current_part_transform, glm::vec3(p0.x, -p0.y - 1.0, 0.0));
        current_part_transform = glm::rotate(current_part_transform, rot, glm::vec3(0.0, 0.0, 1.0));
        current_part_transform =
          glm::scale(current_part_transform, glm::vec3(scale.x, scale.y, 0.0));

        shader_.SetMatrix4("transform", current_part_transform);
        shader_.SetVec4("color", glm::vec4(1.0F, 1.0F, 1.0F, 1.0F));

        Renderer::BindTexture(sprite_texture);
        Renderer::DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, std::nullopt);
    }
}

bool SoldierRenderer::IsSoldierPartTypeVisible(Sprites::SoldierPartType soldier_part_type,
                                               const Soldier& soldier,
                                               bool part_base_visibility)
{
    bool has_blood = false; // TODO: need to get real value
    if (has_blood) {
        switch (soldier_part_type) {
            case Sprites::SoldierPartType::RannyUdo:
            case Sprites::SoldierPartType::RannyNoga:
            case Sprites::SoldierPartType::RannyRamie:
            case Sprites::SoldierPartType::RannyReka:
            case Sprites::SoldierPartType::RannyKlata:
            case Sprites::SoldierPartType::RannyBiodro:
            case Sprites::SoldierPartType::RannyMorda:
                return true;
            default:
                break;
        }
    }

    if (soldier.control.jets && soldier.jets_count > 0) {
        switch (soldier_part_type) {
            case Sprites::SoldierPartType::Stopa:
                return false;
            case Sprites::SoldierPartType::Lecistopa:
                return true;
            default:
                break;
        }
    }

    if (soldier.vest > 0.0F && soldier_part_type == Sprites::SoldierPartType::Kamizelka) {
        return true;
    }

    return part_base_visibility;
}

// TODO: rework this to something more readable
bool SoldierRenderer::IsPrimaryWeaponTypeVisible(
  Sprites::SoldierPartPrimaryWeaponType soldier_part_type,
  const Soldier& soldier,
  bool part_base_visibility)
{
    auto primary_weapon_type = soldier.GetPrimaryWeapon().GetWeaponParameters().kind;
    int ammo = soldier.GetPrimaryWeapon().GetAmmoCount();
    std::uint16_t reload_time_count = soldier.GetPrimaryWeapon().GetReloadTimeCount();

    if (primary_weapon_type == WeaponType::Minigun) {
        if (soldier_part_type == Sprites::SoldierPartPrimaryWeaponType::Minigun) {
            return true;
        }

        if (ammo > 0 || (ammo == 0 && reload_time_count < 65)) {
            if (soldier_part_type == Sprites::SoldierPartPrimaryWeaponType::MinigunClip) {
                return true;
            }
        }

        if (soldier.fired > 0) {
            if (soldier_part_type == Sprites::SoldierPartPrimaryWeaponType::MinigunFire) {
                return true;
            }
        }
    } else if (primary_weapon_type == WeaponType::Bow ||
               primary_weapon_type == WeaponType::FlameBow) {
        if (ammo == 0) {
            if (soldier_part_type == Sprites::SoldierPartPrimaryWeaponType::BowArrowReload) {
                return true;
            }
        } else {
            if (soldier_part_type == Sprites::SoldierPartPrimaryWeaponType::BowArrow) {
                return true;
            }
        }

        if (soldier.body_animation.GetType() == AnimationType::ReloadBow) {
            if (soldier_part_type == Sprites::SoldierPartPrimaryWeaponType::BowReload) {
                return true;
            }
            if (soldier_part_type == Sprites::SoldierPartPrimaryWeaponType::BowStringReload) {
                return true;
            }
        } else {
            if (soldier_part_type == Sprites::SoldierPartPrimaryWeaponType::Bow) {
                return true;
            }
            if (soldier_part_type == Sprites::SoldierPartPrimaryWeaponType::BowString) {
                return true;
            }
        }

        if (soldier.fired > 0) {
            if (soldier_part_type == Sprites::SoldierPartPrimaryWeaponType::BowFire) {
                return true;
            }
        }
    } else if (!soldier.dead_meat) {
        std::array<WeaponType, 14> weapon_types{
            WeaponType::DesertEagles, WeaponType::MP5,     WeaponType::Ak74,  WeaponType::SteyrAUG,
            WeaponType::Spas12,       WeaponType::Ruger77, WeaponType::M79,   WeaponType::Barrett,
            WeaponType::Minimi,       WeaponType::USSOCOM, WeaponType::Knife, WeaponType::Chainsaw,
            WeaponType::LAW,          WeaponType::Flamer
        };

        std::array<Sprites::SoldierPartPrimaryWeaponType, 14> soldier_part_primary_weapon_types{
            Sprites::SoldierPartPrimaryWeaponType::Deagles,
            Sprites::SoldierPartPrimaryWeaponType::Mp5,
            Sprites::SoldierPartPrimaryWeaponType::Ak74,
            Sprites::SoldierPartPrimaryWeaponType::Steyr,
            Sprites::SoldierPartPrimaryWeaponType::Spas,
            Sprites::SoldierPartPrimaryWeaponType::Ruger,
            Sprites::SoldierPartPrimaryWeaponType::M79,
            Sprites::SoldierPartPrimaryWeaponType::Barrett,
            Sprites::SoldierPartPrimaryWeaponType::Minimi,
            Sprites::SoldierPartPrimaryWeaponType::Socom,
            Sprites::SoldierPartPrimaryWeaponType::Knife,
            Sprites::SoldierPartPrimaryWeaponType::Chainsaw,
            Sprites::SoldierPartPrimaryWeaponType::Law,
            Sprites::SoldierPartPrimaryWeaponType::Flamer
        };

        if (std::ranges::contains(soldier_part_primary_weapon_types, soldier_part_type)) {
            for (int i = 0; i < (int)soldier_part_primary_weapon_types.size(); i++) {
                if (soldier_part_primary_weapon_types.at(i) == soldier_part_type) {
                    return weapon_types.at(i) == primary_weapon_type;
                }
            }
        }

        std::array<Sprites::SoldierPartPrimaryWeaponType, 14>
          soldier_part_primary_weapon_clip_types{
              Sprites::SoldierPartPrimaryWeaponType::DeaglesClip,
              Sprites::SoldierPartPrimaryWeaponType::Mp5Clip,
              Sprites::SoldierPartPrimaryWeaponType::Ak74Clip,
              Sprites::SoldierPartPrimaryWeaponType::SteyrClip,
              Sprites::SoldierPartPrimaryWeaponType::SpasClip,
              Sprites::SoldierPartPrimaryWeaponType::RugerClip,
              Sprites::SoldierPartPrimaryWeaponType::M79Clip,
              Sprites::SoldierPartPrimaryWeaponType::BarrettClip,
              Sprites::SoldierPartPrimaryWeaponType::MinimiClip,
              Sprites::SoldierPartPrimaryWeaponType::SocomClip,
              Sprites::SoldierPartPrimaryWeaponType::KnifeClip,
              Sprites::SoldierPartPrimaryWeaponType::ChainsawClip,
              Sprites::SoldierPartPrimaryWeaponType::LawClip,
              Sprites::SoldierPartPrimaryWeaponType::FlamerClip
          };

        auto reload_count = soldier.GetPrimaryWeapon().GetReloadTimeCount();
        auto clip_in_time = soldier.GetPrimaryWeapon().GetClipInTime();
        auto clip_out_time = soldier.GetPrimaryWeapon().GetClipOutTime();

        if (std::ranges::contains(soldier_part_primary_weapon_clip_types, soldier_part_type)) {
            for (int i = 0; i < (int)soldier_part_primary_weapon_clip_types.size(); i++) {
                if (soldier_part_primary_weapon_clip_types.at(i) == soldier_part_type) {
                    if (weapon_types.at(i) == primary_weapon_type) {
                        if (ammo > 0 || (ammo == 0 && (reload_count < clip_in_time ||
                                                       reload_count > clip_out_time))) {
                            return true;
                        }
                    }
                }
            }
        }

        std::array<Sprites::SoldierPartPrimaryWeaponType, 14>
          soldier_part_primary_weapon_fire_types{
              Sprites::SoldierPartPrimaryWeaponType::DeaglesFire,
              Sprites::SoldierPartPrimaryWeaponType::Mp5Fire,
              Sprites::SoldierPartPrimaryWeaponType::Ak74Fire,
              Sprites::SoldierPartPrimaryWeaponType::SteyrFire,
              Sprites::SoldierPartPrimaryWeaponType::SpasFire,
              Sprites::SoldierPartPrimaryWeaponType::RugerFire,
              Sprites::SoldierPartPrimaryWeaponType::M79Fire,
              Sprites::SoldierPartPrimaryWeaponType::BarrettFire,
              Sprites::SoldierPartPrimaryWeaponType::MinimiFire,
              Sprites::SoldierPartPrimaryWeaponType::SocomFire,
              Sprites::SoldierPartPrimaryWeaponType::KnifeFire,
              Sprites::SoldierPartPrimaryWeaponType::ChainsawFire,
              Sprites::SoldierPartPrimaryWeaponType::LawFire,
              Sprites::SoldierPartPrimaryWeaponType::FlamerFire
          };

        if (std::ranges::contains(soldier_part_primary_weapon_fire_types, soldier_part_type)) {
            for (int i = 0; i < (int)soldier_part_primary_weapon_fire_types.size(); i++) {
                if (soldier_part_primary_weapon_fire_types.at(i) == soldier_part_type) {
                    if (weapon_types.at(i) == primary_weapon_type) {
                        if (soldier.fired > 0) {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return part_base_visibility;
}

bool SoldierRenderer::IsSecondaryWeaponTypeVisible(
  Sprites::SoldierPartSecondaryWeaponType soldier_part_type,
  const Soldier& soldier)
{
    auto secondary_weapon_type = soldier.GetSecondaryWeapon().GetWeaponParameters().kind;
    switch (soldier_part_type) {
        case Sprites::SoldierPartSecondaryWeaponType::Deagles:
            return secondary_weapon_type == WeaponType::DesertEagles;
        case Sprites::SoldierPartSecondaryWeaponType::Mp5:
            return secondary_weapon_type == WeaponType::MP5;
        case Sprites::SoldierPartSecondaryWeaponType::Ak74:
            return secondary_weapon_type == WeaponType::Ak74;
        case Sprites::SoldierPartSecondaryWeaponType::Steyr:
            return secondary_weapon_type == WeaponType::SteyrAUG;
        case Sprites::SoldierPartSecondaryWeaponType::Spas:
            return secondary_weapon_type == WeaponType::Spas12;
        case Sprites::SoldierPartSecondaryWeaponType::Ruger:
            return secondary_weapon_type == WeaponType::Ruger77;
        case Sprites::SoldierPartSecondaryWeaponType::M79:
            return secondary_weapon_type == WeaponType::M79;
        case Sprites::SoldierPartSecondaryWeaponType::Barrett:
            return secondary_weapon_type == WeaponType::Barrett;
        case Sprites::SoldierPartSecondaryWeaponType::Minimi:
            return secondary_weapon_type == WeaponType::Minimi;
        case Sprites::SoldierPartSecondaryWeaponType::Minigun:
            return secondary_weapon_type == WeaponType::Minigun;
        case Sprites::SoldierPartSecondaryWeaponType::Socom:
            return secondary_weapon_type == WeaponType::USSOCOM;
        case Sprites::SoldierPartSecondaryWeaponType::Knife:
            return secondary_weapon_type == WeaponType::Knife;
        case Sprites::SoldierPartSecondaryWeaponType::Chainsaw:
            return secondary_weapon_type == WeaponType::Chainsaw;
        case Sprites::SoldierPartSecondaryWeaponType::Law:
            return secondary_weapon_type == WeaponType::LAW;
        case Sprites::SoldierPartSecondaryWeaponType::Flamebow:
            return secondary_weapon_type == WeaponType::FlameBow;
        case Sprites::SoldierPartSecondaryWeaponType::Bow:
            return secondary_weapon_type == WeaponType::Bow;
        case Sprites::SoldierPartSecondaryWeaponType::Flamer:
            return secondary_weapon_type == WeaponType::Flamer;
    }
}

bool SoldierRenderer::IsTertiaryWeaponTypeVisible(
  Sprites::SoldierPartTertiaryWeaponType soldier_part_type,
  const Soldier& soldier)
{
    auto tertiary_weapon_type = soldier.GetTertiaryWeapon().GetWeaponParameters().kind;
    int ammo = soldier.GetTertiaryWeapon().GetAmmoCount();
    if (tertiary_weapon_type == WeaponType::FragGrenade) {
        std::vector<Sprites::SoldierPartTertiaryWeaponType> sprite_types{
            Sprites::SoldierPartTertiaryWeaponType::FragGrenade1,
            Sprites::SoldierPartTertiaryWeaponType::FragGrenade2,
            Sprites::SoldierPartTertiaryWeaponType::FragGrenade3,
            Sprites::SoldierPartTertiaryWeaponType::FragGrenade4,
            Sprites::SoldierPartTertiaryWeaponType::FragGrenade5
        };

        int n = 0;
        if (soldier.body_animation.GetType() == AnimationType::Throw) {
            n = std::min(5, ammo - 1);
        } else {
            n = std::min(5, ammo);
        }

        for (int i = 0; i < (int)sprite_types.size(); i++) {
            if (sprite_types[i] == soldier_part_type) {
                return i < n;
            }
        }
    } else if (tertiary_weapon_type == WeaponType::ClusterGrenade) {
        std::vector<Sprites::SoldierPartTertiaryWeaponType> sprite_types{
            Sprites::SoldierPartTertiaryWeaponType::ClusterGrenade1,
            Sprites::SoldierPartTertiaryWeaponType::ClusterGrenade2,
            Sprites::SoldierPartTertiaryWeaponType::ClusterGrenade3,
            Sprites::SoldierPartTertiaryWeaponType::ClusterGrenade4,
            Sprites::SoldierPartTertiaryWeaponType::ClusterGrenade5
        };

        int n = 0;
        if (soldier.body_animation.GetType() == AnimationType::Throw) {
            n = std::min(5, ammo - 1);
        } else {
            n = std::min(5, ammo);
        }

        for (int i = 0; i < (int)sprite_types.size(); i++) {
            if (sprite_types[i] == soldier_part_type) {
                return i < n;
            }
        }
    }

    return false;
}
} // namespace Soldat
