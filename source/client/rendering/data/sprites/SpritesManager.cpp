#include "SpritesManager.hpp"

#include "rendering/data/Texture.hpp"

#include <iostream>
#include <memory>
#include <variant>
#include <vector>
#include <optional>

namespace Soldat::Sprites
{
SpriteManager::SpriteManager()
{
    std::cout << "Init Sprites" << std::endl;
    // clang-format off
    auto* v = new TList();
    v->push_back({ SoldierPartSecondaryWeaponType::Deagles, nullptr});
    v->push_back({ SoldierPartSecondaryWeaponType::Mp5, std::make_unique<SoldierPartData>("weapons-gfx/mp5.png", glm::uvec2(5, 10), glm::vec2(0.300, 0.300), false, "weapons-gfx/mp5-2.png", false, 0.0F, SoldierColor::None, SoldierAlpha::Base)});
    v->push_back({ SoldierPartSecondaryWeaponType::Ak74, std::make_unique<SoldierPartData>("weapons-gfx/ak74.png", glm::uvec2(5, 10), glm::vec2(0.300, 0.250), false, "weapons-gfx/ak74-2.png", false, 0.0F, SoldierColor::None, SoldierAlpha::Base)});
    v->push_back({ SoldierPartSecondaryWeaponType::Steyr, std::make_unique<SoldierPartData>("weapons-gfx/steyraug.png", glm::uvec2(5, 10), glm::vec2(0.300, 0.500), false, "weapons-gfx/steyraug-2.png", false, 0.0F, SoldierColor::None, SoldierAlpha::Base)});
    v->push_back({ SoldierPartSecondaryWeaponType::Spas, std::make_unique<SoldierPartData>("weapons-gfx/spas12.png", glm::uvec2(5, 10), glm::vec2(0.300, 0.300), false, "weapons-gfx/spas12-2.png", false, 0.0F, SoldierColor::None, SoldierAlpha::Base)});
    v->push_back({ SoldierPartSecondaryWeaponType::Ruger, std::make_unique<SoldierPartData>("weapons-gfx/ruger77.png", glm::uvec2(5, 10), glm::vec2(0.300, 0.300), false, "weapons-gfx/ruger77-2.png", false, 0.0F, SoldierColor::None, SoldierAlpha::Base)});
    v->push_back({ SoldierPartSecondaryWeaponType::M79, std::make_unique<SoldierPartData>("weapons-gfx/m79.png", glm::uvec2(5, 10), glm::vec2(0.300, 0.350), false, "weapons-gfx/m79-2.png", false, 0.0F, SoldierColor::None, SoldierAlpha::Base)});
    v->push_back({ SoldierPartSecondaryWeaponType::Barrett, std::make_unique<SoldierPartData>("weapons-gfx/barretm82.png", glm::uvec2(5, 10), glm::vec2(0.300, 0.350), false, "weapons-gfx/barretm82-2.png", false, 0.0F, SoldierColor::None, SoldierAlpha::Base)});
    v->push_back({ SoldierPartSecondaryWeaponType::Minimi, std::make_unique<SoldierPartData>("weapons-gfx/m249.png", glm::uvec2(5, 10), glm::vec2(0.300, 0.350), false, "weapons-gfx/m249-2.png", false, 0.0F, SoldierColor::None, SoldierAlpha::Base)});
    v->push_back({ SoldierPartSecondaryWeaponType::Minigun, std::make_unique<SoldierPartData>("weapons-gfx/minigun.png", glm::uvec2(5, 10), glm::vec2(0.200, 0.500), false, "weapons-gfx/minigun-2.png", false, 0.0F, SoldierColor::None, SoldierAlpha::Base)});
    v->push_back({ SoldierPartSecondaryWeaponType::Socom, nullptr});
    v->push_back({ SoldierPartSecondaryWeaponType::Knife, nullptr});
    v->push_back({ SoldierPartSecondaryWeaponType::Chainsaw, std::make_unique<SoldierPartData>("weapons-gfx/chainsaw.png", glm::uvec2(5, 10), glm::vec2(0.250, 0.500), false, "weapons-gfx/chainsaw2.png", false, 0.0F, SoldierColor::None, SoldierAlpha::Base)});
    v->push_back({ SoldierPartSecondaryWeaponType::Law, std::make_unique<SoldierPartData>("weapons-gfx/law.png", glm::uvec2(5, 10), glm::vec2(0.300, 0.450), false, "weapons-gfx/law-2.png", false, 0.0F, SoldierColor::None, SoldierAlpha::Base)});
    v->push_back({ SoldierPartSecondaryWeaponType::Flamebow, nullptr});
    v->push_back({ SoldierPartSecondaryWeaponType::Bow, nullptr});
    v->push_back({ SoldierPartSecondaryWeaponType::Flamer, std::make_unique<SoldierPartData>("weapons-gfx/flamer.png", glm::uvec2(5, 10), glm::vec2(0.300, 0.300), false, "weapons-gfx/flamer-2.png", false, 0.0F, SoldierColor::None, SoldierAlpha::Base)});
    v->push_back({ SoldierPartType::Udo, std::make_unique<SoldierPartData>("gostek-gfx/udo.png", glm::uvec2(6, 3), glm::vec2(0.200, 0.500), true, "gostek-gfx/udo2.png", true, 5.0, SoldierColor::Pants, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyUdo, std::make_unique<SoldierPartData>("gostek-gfx/ranny/udo.png", glm::uvec2(6, 3), glm::vec2(0.200, 0.500), false, "gostek-gfx/ranny/udo2.png", true, 5.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Stopa, std::make_unique<SoldierPartData>("gostek-gfx/stopa.png", glm::uvec2(2, 18), glm::vec2(0.350, 0.350), true, "gostek-gfx/stopa2.png", true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Lecistopa, std::make_unique<SoldierPartData>("gostek-gfx/lecistopa.png", glm::uvec2(2, 18), glm::vec2(0.350, 0.350), false, "gostek-gfx/lecistopa2.png", true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Noga, std::make_unique<SoldierPartData>("gostek-gfx/noga.png", glm::uvec2(3, 2), glm::vec2(0.150, 0.550), true, "gostek-gfx/noga2.png", true, 0.0, SoldierColor::Pants, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyNoga, std::make_unique<SoldierPartData>("gostek-gfx/ranny/noga.png", glm::uvec2(3, 2), glm::vec2(0.150, 0.550), false, "gostek-gfx/ranny/noga2.png", true, 0.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Ramie, std::make_unique<SoldierPartData>("gostek-gfx/ramie.png", glm::uvec2(11, 14), glm::vec2(0.000, 0.500), true, "gostek-gfx/ramie2.png", true, 0.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyRamie, std::make_unique<SoldierPartData>("gostek-gfx/ranny/ramie.png", glm::uvec2(11, 14), glm::vec2(0.000, 0.500), false, "gostek-gfx/ranny/ramie2.png", true, 0.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Reka, std::make_unique<SoldierPartData>("gostek-gfx/reka.png", glm::uvec2(14, 15), glm::vec2(0.000, 0.500), true, std::nullopt, true, 5.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyReka, std::make_unique<SoldierPartData>("gostek-gfx/ranny/reka.png", glm::uvec2(14, 15), glm::vec2(0.000, 0.500), false, "gostek-gfx/ranny/reka2.png", true, 5.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Dlon, std::make_unique<SoldierPartData>("gostek-gfx/dlon.png", glm::uvec2(15, 19), glm::vec2(0.000, 0.400), true, "gostek-gfx/dlon2.png", true, 0.0, SoldierColor::Skin, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Helm, std::make_unique<SoldierPartData>("gostek-gfx/helm.png", glm::uvec2(15, 19), glm::vec2(0.000, 0.500), false, "gostek-gfx/helm2.png", true, 0.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Kap, std::make_unique<SoldierPartData>("gostek-gfx/kap.png", glm::uvec2(15, 19), glm::vec2(0.100, 0.400), false, "gostek-gfx/kap2.png", true, 0.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Udo, std::make_unique<SoldierPartData>("gostek-gfx/udo.png", glm::uvec2(5, 4), glm::vec2(0.200, 0.650), true, "gostek-gfx/udo2.png", true, 5.0, SoldierColor::Pants, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyUdo, std::make_unique<SoldierPartData>("gostek-gfx/ranny/udo.png", glm::uvec2(5, 4), glm::vec2(0.200, 0.650), false, "gostek-gfx/ranny/udo2.png", true, 5.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Stopa, std::make_unique<SoldierPartData>("gostek-gfx/stopa.png", glm::uvec2(1, 17), glm::vec2(0.350, 0.350), true, "gostek-gfx/stopa2.png", true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Lecistopa, std::make_unique<SoldierPartData>("gostek-gfx/lecistopa.png", glm::uvec2(1, 17), glm::vec2(0.350, 0.350), false, "gostek-gfx/lecistopa2.png", true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Noga, std::make_unique<SoldierPartData>("gostek-gfx/noga.png", glm::uvec2(4, 1), glm::vec2(0.150, 0.550), true, "gostek-gfx/noga2.png", true, 0.0, SoldierColor::Pants, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyNoga, std::make_unique<SoldierPartData>("gostek-gfx/ranny/noga.png", glm::uvec2(4, 1), glm::vec2(0.150, 0.550), false, "gostek-gfx/ranny/noga2.png", true, 0.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Klata, std::make_unique<SoldierPartData>("gostek-gfx/klata.png", glm::uvec2(10, 11), glm::vec2(0.100, 0.300), true, "gostek-gfx/klata2.png", true, 0.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Kamizelka, std::make_unique<SoldierPartData>("gostek-gfx/kamizelka.png", glm::uvec2(10, 11), glm::vec2(0.100, 0.300), false, "gostek-gfx/kamizelka2.png", true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyKlata, std::make_unique<SoldierPartData>("gostek-gfx/ranny/klata.png", glm::uvec2(10, 11), glm::vec2(0.100, 0.300), false, "gostek-gfx/ranny/klata2.png", true, 0.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Biodro, std::make_unique<SoldierPartData>("gostek-gfx/biodro.png", glm::uvec2(5, 6), glm::vec2(0.250, 0.600), true, "gostek-gfx/biodro2.png", true, 0.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyBiodro, std::make_unique<SoldierPartData>("gostek-gfx/ranny/biodro.png", glm::uvec2(5, 6), glm::vec2(0.250, 0.600), false, "gostek-gfx/ranny/biodro2.png", true, 0.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Morda, std::make_unique<SoldierPartData>("gostek-gfx/morda.png", glm::uvec2(9, 12), glm::vec2(0.000, 0.500), true, "gostek-gfx/morda2.png", true, 0.0, SoldierColor::Skin, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyMorda, std::make_unique<SoldierPartData>("gostek-gfx/ranny/morda.png", glm::uvec2(9, 12), glm::vec2(0.000, 0.500), false, "gostek-gfx/ranny/morda2.png", true, 0.0, SoldierColor::Headblood, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Morda, std::make_unique<SoldierPartData>("gostek-gfx/morda.png", glm::uvec2(9, 12), glm::vec2(0.500, 0.500), false, "gostek-gfx/morda2.png", true, 0.0, SoldierColor::Skin, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyMorda, std::make_unique<SoldierPartData>("gostek-gfx/ranny/morda.png", glm::uvec2(9, 12), glm::vec2(0.500, 0.500), false, "gostek-gfx/ranny/morda2.png", true, 0.0, SoldierColor::Headblood, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Hair3, std::make_unique<SoldierPartData>("gostek-gfx/hair3.png", glm::uvec2(9, 12), glm::vec2(0.000, 0.500), false, "gostek-gfx/hair32.png", true, 0.0, SoldierColor::Hair, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Helm, std::make_unique<SoldierPartData>("gostek-gfx/helm.png", glm::uvec2(9, 12), glm::vec2(0.000, 0.500), false, "gostek-gfx/helm2.png", true, 0.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Kap, std::make_unique<SoldierPartData>("gostek-gfx/kap.png", glm::uvec2(9, 12), glm::vec2(0.000, 0.500), false, "gostek-gfx/kap2.png", true, 0.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Badge, std::make_unique<SoldierPartData>("gostek-gfx/badge.png", glm::uvec2(9, 12), glm::vec2(0.000, 0.500), false, "gostek-gfx/badge2.png", true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Hair1, std::make_unique<SoldierPartData>("gostek-gfx/hair1.png", glm::uvec2(9, 12), glm::vec2(0.000, 0.500), false, "gostek-gfx/hair12.png", true, 0.0, SoldierColor::Hair, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Dred, std::make_unique<SoldierPartData>("gostek-gfx/dred.png", glm::uvec2(23, 24), glm::vec2(0.000, 1.220), false, std::nullopt, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Dred, std::make_unique<SoldierPartData>("gostek-gfx/dred.png", glm::uvec2(23, 24), glm::vec2(0.100, 0.500), false, std::nullopt, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Dred, std::make_unique<SoldierPartData>("gostek-gfx/dred.png", glm::uvec2(23, 24), glm::vec2(0.040, -0.300), false, std::nullopt, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Dred, std::make_unique<SoldierPartData>("gostek-gfx/dred.png", glm::uvec2(23, 24), glm::vec2(0.000, -0.900), false, std::nullopt, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Dred, std::make_unique<SoldierPartData>("gostek-gfx/dred.png", glm::uvec2(23, 24), glm::vec2(-0.200, -1.350), false, std::nullopt, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Hair2, std::make_unique<SoldierPartData>("gostek-gfx/hair2.png", glm::uvec2(9, 12), glm::vec2(0.000, 0.500), false, "gostek-gfx/hair22.png", true, 0.0, SoldierColor::Hair, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Hair4, std::make_unique<SoldierPartData>("gostek-gfx/hair4.png", glm::uvec2(9, 12), glm::vec2(0.000, 0.500), false, "gostek-gfx/hair42.png", true, 0.0, SoldierColor::Hair, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Cygaro, std::make_unique<SoldierPartData>("gostek-gfx/cygaro.png", glm::uvec2(9, 12), glm::vec2(-0.125, 0.400), false, "gostek-gfx/cygaro2.png", true, 0.0, SoldierColor::Cygar, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Lancuch, std::make_unique<SoldierPartData>("gostek-gfx/lancuch.png", glm::uvec2(10, 22), glm::vec2(0.100, 0.500), false, std::nullopt, true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Lancuch, std::make_unique<SoldierPartData>("gostek-gfx/lancuch.png", glm::uvec2(11, 22), glm::vec2(0.100, 0.500), false, std::nullopt, true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Metal, std::make_unique<SoldierPartData>("gostek-gfx/metal.png", glm::uvec2(22, 21), glm::vec2(0.500, 0.700), false, "gostek-gfx/metal2.png", true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Zlotylancuch, std::make_unique<SoldierPartData>("gostek-gfx/zlotylancuch.png", glm::uvec2(10, 22), glm::vec2(0.100, 0.500), false, std::nullopt, true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Zlotylancuch, std::make_unique<SoldierPartData>("gostek-gfx/zlotylancuch.png", glm::uvec2(11, 22), glm::vec2(0.100, 0.500), false, std::nullopt, true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Zloto, std::make_unique<SoldierPartData>("gostek-gfx/zloto.png", glm::uvec2(22, 21), glm::vec2(0.500, 0.500), false, "gostek-gfx/zloto2.png", true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartTertiaryWeaponType::FragGrenade1, std::make_unique<SoldierPartData>("weapons-gfx/frag-grenade.png", glm::uvec2( 5,  6), glm::vec2( 0.500,  0.100), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Nades) });
    v->push_back({ SoldierPartTertiaryWeaponType::FragGrenade2, std::make_unique<SoldierPartData>("weapons-gfx/frag-grenade.png", glm::uvec2( 5,  6), glm::vec2( 0.500,  0.100), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Nades) });
    v->push_back({ SoldierPartTertiaryWeaponType::FragGrenade3, std::make_unique<SoldierPartData>("weapons-gfx/frag-grenade.png", glm::uvec2( 5,  6), glm::vec2( 0.500,  0.100), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Nades) });
    v->push_back({ SoldierPartTertiaryWeaponType::FragGrenade4, std::make_unique<SoldierPartData>("weapons-gfx/frag-grenade.png", glm::uvec2( 5,  6), glm::vec2( 0.500,  0.100), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Nades) });
    v->push_back({ SoldierPartTertiaryWeaponType::FragGrenade5, std::make_unique<SoldierPartData>("weapons-gfx/frag-grenade.png", glm::uvec2( 5,  6), glm::vec2( 0.500,  0.100), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Nades) });
    v->push_back({ SoldierPartTertiaryWeaponType::ClusterGrenade1, std::make_unique<SoldierPartData>("weapons-gfx/cluster-grenade.png", glm::uvec2( 5,  6), glm::vec2( 0.500,  0.300), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Nades) });
    v->push_back({ SoldierPartTertiaryWeaponType::ClusterGrenade2, std::make_unique<SoldierPartData>("weapons-gfx/cluster-grenade.png", glm::uvec2( 5,  6), glm::vec2( 0.500,  0.300), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Nades) });
    v->push_back({ SoldierPartTertiaryWeaponType::ClusterGrenade3, std::make_unique<SoldierPartData>("weapons-gfx/cluster-grenade.png", glm::uvec2( 5,  6), glm::vec2( 0.500,  0.300), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Nades) });
    v->push_back({ SoldierPartTertiaryWeaponType::ClusterGrenade4, std::make_unique<SoldierPartData>("weapons-gfx/cluster-grenade.png", glm::uvec2( 5,  6), glm::vec2( 0.500,  0.300), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Nades) });
    v->push_back({ SoldierPartTertiaryWeaponType::ClusterGrenade5, std::make_unique<SoldierPartData>("weapons-gfx/cluster-grenade.png", glm::uvec2( 5,  6), glm::vec2( 0.500,  0.300), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Nades) });
    v->push_back({ SoldierPartPrimaryWeaponType::Deagles, std::make_unique<SoldierPartData>("weapons-gfx/deserteagle.png", glm::uvec2(16, 15), glm::vec2( 0.100,  0.800), false, "weapons-gfx/deserteagle-2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::DeaglesClip, std::make_unique<SoldierPartData>("weapons-gfx/deserteagle-clip.png", glm::uvec2(16, 15), glm::vec2( 0.100,  0.800), false, "weapons-gfx/deserteagle-clip2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::DeaglesFire, std::make_unique<SoldierPartData>("weapons-gfx/eagles-fire.png", glm::uvec2(16, 15), glm::vec2(-0.500,  1.000), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::Mp5, std::make_unique<SoldierPartData>("weapons-gfx/mp5.png", glm::uvec2(16, 15), glm::vec2( 0.150,  0.600), false, "weapons-gfx/mp5-2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::Mp5Clip, std::make_unique<SoldierPartData>("weapons-gfx/mp5-clip.png", glm::uvec2(16, 15), glm::vec2( 0.150,  0.600), false, "weapons-gfx/mp5-clip2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::Mp5Fire, std::make_unique<SoldierPartData>("weapons-gfx/mp5-fire.png", glm::uvec2(16, 15), glm::vec2( -0.65,  0.850), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::Ak74, std::make_unique<SoldierPartData>("weapons-gfx/ak74.png", glm::uvec2(16, 15), glm::vec2( 0.150,  0.500), false, "weapons-gfx/ak74-2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::Ak74Clip, std::make_unique<SoldierPartData>("weapons-gfx/ak74-clip.png", glm::uvec2(16, 15), glm::vec2( 0.150,  0.500), false, "weapons-gfx/ak74-clip2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::Ak74Fire, std::make_unique<SoldierPartData>("weapons-gfx/ak74-fire.png", glm::uvec2(16, 15), glm::vec2( -0.37,  0.800), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::Steyr, std::make_unique<SoldierPartData>("weapons-gfx/steyraug.png", glm::uvec2(16, 15), glm::vec2( 0.200,  0.600), false, "weapons-gfx/steyraug-2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::SteyrClip, std::make_unique<SoldierPartData>("weapons-gfx/steyraug-clip.png", glm::uvec2(16, 15), glm::vec2( 0.200,  0.600), false, "weapons-gfx/steyraug-clip2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::SteyrFire, std::make_unique<SoldierPartData>("weapons-gfx/steyraug-fire.png", glm::uvec2(16, 15), glm::vec2( -0.24,  0.750), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::Spas, std::make_unique<SoldierPartData>("weapons-gfx/spas12.png", glm::uvec2(16, 15), glm::vec2( 0.100,  0.600), false, "weapons-gfx/spas12-2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::SpasClip, nullptr});
    v->push_back({ SoldierPartPrimaryWeaponType::SpasFire, std::make_unique<SoldierPartData>("weapons-gfx/spas12-fire.png", glm::uvec2(16, 15), glm::vec2(-0.200,  0.900), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::Ruger, std::make_unique<SoldierPartData>("weapons-gfx/ruger77.png", glm::uvec2(16, 15), glm::vec2( 0.100,  0.700), false, "weapons-gfx/ruger77-2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::RugerClip, nullptr});
    v->push_back({ SoldierPartPrimaryWeaponType::RugerFire, std::make_unique<SoldierPartData>("weapons-gfx/ruger77-fire.png", glm::uvec2(16, 15), glm::vec2( -0.35,  0.850), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::M79, std::make_unique<SoldierPartData>("weapons-gfx/m79.png", glm::uvec2(16, 15), glm::vec2( 0.100,  0.700), false, "weapons-gfx/m79-2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::M79Clip, std::make_unique<SoldierPartData>("weapons-gfx/m79-clip.png", glm::uvec2(16, 15), glm::vec2( 0.100,  0.700), false, "weapons-gfx/m79-clip2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::M79Fire, std::make_unique<SoldierPartData>("weapons-gfx/m79-fire.png", glm::uvec2(16, 15), glm::vec2(-0.400,  0.800), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::Barrett, std::make_unique<SoldierPartData>("weapons-gfx/barretm82.png", glm::uvec2(16, 15), glm::vec2( 0.150,  0.700), false, "weapons-gfx/barretm82-2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::BarrettClip, std::make_unique<SoldierPartData>("weapons-gfx/barretm82-clip.png", glm::uvec2(16, 15), glm::vec2( 0.150,  0.700), false, "weapons-gfx/barretm82-clip2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::BarrettFire, std::make_unique<SoldierPartData>("weapons-gfx/barret-fire.png", glm::uvec2(16, 15), glm::vec2( -0.15,  0.800), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::Minimi, std::make_unique<SoldierPartData>("weapons-gfx/m249.png", glm::uvec2(16, 15), glm::vec2( 0.150,  0.600), false, "weapons-gfx/m249-2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::MinimiClip, std::make_unique<SoldierPartData>("weapons-gfx/m249-clip.png", glm::uvec2(16, 15), glm::vec2( 0.150,  0.600), false, "weapons-gfx/m249-clip2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::MinimiFire, std::make_unique<SoldierPartData>("weapons-gfx/m249-fire.png", glm::uvec2(16, 15), glm::vec2(-0.200,  0.900), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::MinigunClip, std::make_unique<SoldierPartData>("weapons-gfx/minigun-clip.png", glm::uvec2( 8,  7), glm::vec2( 0.500,  0.100), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::Minigun, std::make_unique<SoldierPartData>("weapons-gfx/minigun.png", glm::uvec2(16, 15), glm::vec2( 0.050,  0.500), false, "weapons-gfx/minigun-2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::MinigunFire, std::make_unique<SoldierPartData>("weapons-gfx/minigun-fire.png", glm::uvec2(16, 15), glm::vec2(-0.200,  0.450), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::Socom, std::make_unique<SoldierPartData>("weapons-gfx/colt1911.png", glm::uvec2(16, 15), glm::vec2( 0.200,  0.550), false, "weapons-gfx/colt1911-2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::SocomClip, std::make_unique<SoldierPartData>("weapons-gfx/colt1911-clip.png", glm::uvec2(16, 15), glm::vec2( 0.200,  0.550), false, "weapons-gfx/colt1911-clip2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::SocomFire, std::make_unique<SoldierPartData>("weapons-gfx/colt1911-fire.png", glm::uvec2(16, 15), glm::vec2( -0.24,  0.850), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::Knife, std::make_unique<SoldierPartData>("weapons-gfx/knife.png", glm::uvec2(16, 20), glm::vec2(-0.100,  0.600), false, "weapons-gfx/knife2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::KnifeClip, nullptr});
    v->push_back({ SoldierPartPrimaryWeaponType::KnifeFire, nullptr});
    v->push_back({ SoldierPartPrimaryWeaponType::Chainsaw, std::make_unique<SoldierPartData>("weapons-gfx/chainsaw.png", glm::uvec2(16, 15), glm::vec2( 0.100,  0.500), false, "weapons-gfx/chainsaw2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::ChainsawClip, nullptr});
    v->push_back({ SoldierPartPrimaryWeaponType::ChainsawFire, std::make_unique<SoldierPartData>("weapons-gfx/chainsaw-fire.png", glm::uvec2(16, 15), glm::vec2( 0.000,  0.000), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::Law, std::make_unique<SoldierPartData>("weapons-gfx/law.png", glm::uvec2(16, 15), glm::vec2( 0.100,  0.600), false, "weapons-gfx/law-2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::LawClip, nullptr});
    v->push_back({ SoldierPartPrimaryWeaponType::LawFire, std::make_unique<SoldierPartData>("weapons-gfx/law-fire.png", glm::uvec2(16, 15), glm::vec2(-0.100,  0.550), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::Bow, std::make_unique<SoldierPartData>("weapons-gfx/bow.png", glm::uvec2(16, 15), glm::vec2(-0.400,  0.550), false, "weapons-gfx/bow-2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::BowArrow, std::make_unique<SoldierPartData>("weapons-gfx/bow-a.png", glm::uvec2(16, 15), glm::vec2( 0.000,  0.550), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::BowString, std::make_unique<SoldierPartData>("weapons-gfx/bow-s.png", glm::uvec2(16, 15), glm::vec2(-0.400,  0.550), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::BowReload, std::make_unique<SoldierPartData>("weapons-gfx/bow.png", glm::uvec2(16, 15), glm::vec2(-0.400,  0.550), false, "weapons-gfx/bow-2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::BowArrowReload, std::make_unique<SoldierPartData>("weapons-gfx/arrow.png", glm::uvec2(16, 20), glm::vec2( 0.000,  0.550), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::BowStringReload, std::make_unique<SoldierPartData>("weapons-gfx/bow-s.png", glm::uvec2(16, 15), glm::vec2(-0.400,  0.550), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::BowFire, std::make_unique<SoldierPartData>("weapons-gfx/bow-fire.png", glm::uvec2(16, 15), glm::vec2( 0.000,  0.000), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::Flamer, std::make_unique<SoldierPartData>("weapons-gfx/flamer.png", glm::uvec2(16, 15), glm::vec2( 0.200,  0.700), false, "weapons-gfx/flamer-2.png", false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartPrimaryWeaponType::FlamerClip, nullptr});
    v->push_back({ SoldierPartPrimaryWeaponType::FlamerFire, std::make_unique<SoldierPartData>("weapons-gfx/flamer-fire.png", glm::uvec2(16, 15), glm::vec2( 0.000,  0.000), false, std::nullopt, false, 0.0, SoldierColor::None, SoldierAlpha::Base ) });
    v->push_back({ SoldierPartType::Ramie, std::make_unique<SoldierPartData>("gostek-gfx/ramie.png", glm::uvec2(10, 13), glm::vec2(0.000, 0.600), true, "gostek-gfx/ramie2.png", true, 0.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyRamie, std::make_unique<SoldierPartData>("gostek-gfx/ranny/ramie.png", glm::uvec2(10, 13), glm::vec2(-0.100, 0.500), false, "gostek-gfx/ranny/ramie2.png", true, 0.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Reka, std::make_unique<SoldierPartData>("gostek-gfx/reka.png", glm::uvec2(13, 16), glm::vec2(0.000, 0.600), true, std::nullopt, true, 5.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyReka, std::make_unique<SoldierPartData>("gostek-gfx/ranny/reka.png", glm::uvec2(13, 16), glm::vec2(0.000, 0.600), false, "gostek-gfx/ranny/reka2.png", true, 5.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Dlon, std::make_unique<SoldierPartData>("gostek-gfx/dlon.png", glm::uvec2(16, 20), glm::vec2(0.000, 0.500), true, "gostek-gfx/dlon2.png", true, 0.0, SoldierColor::Skin, SoldierAlpha::Base) });
    soldier_part_type_to_data_ = std::unique_ptr<TList>(v);
    // clang-format on
    std::cout << "Loaded " << soldier_part_type_to_data_->size() << " gostek-gfx assets"
              << std::endl;
}

SpriteManager::~SpriteManager()
{
    soldier_part_type_to_data_.reset(nullptr);
}

const SoldierPartData* SpriteManager::GetSoldierPartData(unsigned int id) const
{
    return (*soldier_part_type_to_data_)[id].second.get();
}
std::variant<SoldierPartType,
             SoldierPartPrimaryWeaponType,
             SoldierPartSecondaryWeaponType,
             SoldierPartTertiaryWeaponType>
SpriteManager::GetSoldierPartDataType(unsigned int id) const
{
    return (*soldier_part_type_to_data_)[id].first;
}
unsigned int SpriteManager::GetSoldierPartCount() const
{
    return soldier_part_type_to_data_->size();
}
} // namespace Soldat::Sprites
