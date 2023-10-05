#include "SpritesManager.hpp"

#include "rendering/data/Texture.hpp"

#include <iostream>
#include <memory>
#include <utility>
#include <variant>
#include <vector>
#include <optional>
#include <algorithm>

namespace Soldat::Sprites
{
SpriteManager::SpriteManager()
{
    std::cout << "Init Sprites" << std::endl;
    std::vector<std::pair<TSpriteKey, std::string>> all_sprite_file_paths{
        { SoldierPartType::Stopa, "gostek-gfx/stopa.png" },
        { SoldierPartType::Stopa2, "gostek-gfx/stopa2.png" },
        { SoldierPartType::Noga, "gostek-gfx/noga.png" },
        { SoldierPartType::Noga2, "gostek-gfx/noga2.png" },
        { SoldierPartType::Udo, "gostek-gfx/udo.png" },
        { SoldierPartType::Udo2, "gostek-gfx/udo2.png" },
        { SoldierPartType::Biodro, "gostek-gfx/biodro.png" },
        { SoldierPartType::Biodro2, "gostek-gfx/biodro2.png" },
        { SoldierPartType::Klata, "gostek-gfx/klata.png" },
        { SoldierPartType::Klata2, "gostek-gfx/klata2.png" },
        { SoldierPartType::Morda, "gostek-gfx/morda.png" },
        { SoldierPartType::Morda2, "gostek-gfx/morda2.png" },
        { SoldierPartType::Ramie, "gostek-gfx/ramie.png" },
        { SoldierPartType::Ramie2, "gostek-gfx/ramie2.png" },
        { SoldierPartType::Reka, "gostek-gfx/reka.png" },
        { SoldierPartType::Reka2, "gostek-gfx/reka2.png" },
        { SoldierPartType::Dlon, "gostek-gfx/dlon.png" },
        { SoldierPartType::Dlon2, "gostek-gfx/dlon2.png" },
        { SoldierPartType::Lancuch, "gostek-gfx/lancuch.png" },
        { SoldierPartType::Helm, "gostek-gfx/helm.png" },
        { SoldierPartType::Helm2, "gostek-gfx/helm2.png" },
        { SoldierPartType::Badge, "gostek-gfx/badge.png" },
        { SoldierPartType::Badge2, "gostek-gfx/badge2.png" },
        { SoldierPartType::Cygaro, "gostek-gfx/cygaro.png" },
        { SoldierPartType::Cygaro2, "gostek-gfx/cygaro2.png" },
        { SoldierPartType::Metal, "gostek-gfx/metal.png" },
        { SoldierPartType::Metal2, "gostek-gfx/metal2.png" },
        { SoldierPartType::Zloto, "gostek-gfx/zloto.png" },
        { SoldierPartType::Zloto2, "gostek-gfx/zloto2.png" },
        { SoldierPartType::Zlotylancuch, "gostek-gfx/zlotylancuch.png" },
        { SoldierPartType::Kap, "gostek-gfx/kap.png" },
        { SoldierPartType::Kap2, "gostek-gfx/kap2.png" },
        { SoldierPartType::Dred, "gostek-gfx/dred.png" },
        { SoldierPartType::Hair1, "gostek-gfx/hair1.png" },
        { SoldierPartType::Hair12, "gostek-gfx/hair12.png" },
        { SoldierPartType::Hair2, "gostek-gfx/hair2.png" },
        { SoldierPartType::Hair22, "gostek-gfx/hair22.png" },
        { SoldierPartType::Hair3, "gostek-gfx/hair3.png" },
        { SoldierPartType::Hair32, "gostek-gfx/hair32.png" },
        { SoldierPartType::Hair4, "gostek-gfx/hair4.png" },
        { SoldierPartType::Hair42, "gostek-gfx/hair42.png" },
        { SoldierPartType::Kamizelka, "gostek-gfx/kamizelka.png" },
        { SoldierPartType::Kamizelka2, "gostek-gfx/kamizelka2.png" },
        { SoldierPartType::RannyKlata, "gostek-gfx/ranny/klata.png" },
        { SoldierPartType::RannyKlata2, "gostek-gfx/ranny/klata2.png" },
        { SoldierPartType::RannyMorda, "gostek-gfx/ranny/morda.png" },
        { SoldierPartType::RannyMorda2, "gostek-gfx/ranny/morda2.png" },
        { SoldierPartType::RannyNoga, "gostek-gfx/ranny/noga.png" },
        { SoldierPartType::RannyNoga2, "gostek-gfx/ranny/noga2.png" },
        { SoldierPartType::RannyRamie, "gostek-gfx/ranny/ramie.png" },
        { SoldierPartType::RannyRamie2, "gostek-gfx/ranny/ramie2.png" },
        { SoldierPartType::RannyReka, "gostek-gfx/ranny/reka.png" },
        { SoldierPartType::RannyReka2, "gostek-gfx/ranny/reka2.png" },
        { SoldierPartType::RannyUdo, "gostek-gfx/ranny/udo.png" },
        { SoldierPartType::RannyUdo2, "gostek-gfx/ranny/udo2.png" },
        { SoldierPartType::RannyBiodro, "gostek-gfx/ranny/biodro.png" },
        { SoldierPartType::RannyBiodro2, "gostek-gfx/ranny/biodro2.png" },
        { SoldierPartType::Lecistopa, "gostek-gfx/lecistopa.png" },
        { SoldierPartType::Lecistopa2, "gostek-gfx/lecistopa2.png" },

        // Soldier Team2 list. Keep it next to the non-team2 list.

        { SoldierPartType::Team2Stopa, "gostek-gfx/team2/stopa.png" },
        { SoldierPartType::Team2Stopa2, "gostek-gfx/team2/stopa2.png" },
        { SoldierPartType::Team2Noga, "gostek-gfx/team2/noga.png" },
        { SoldierPartType::Team2Noga2, "gostek-gfx/team2/noga2.png" },
        { SoldierPartType::Team2Udo, "gostek-gfx/team2/udo.png" },
        { SoldierPartType::Team2Udo2, "gostek-gfx/team2/udo2.png" },
        { SoldierPartType::Team2Biodro, "gostek-gfx/team2/biodro.png" },
        { SoldierPartType::Team2Biodro2, "gostek-gfx/team2/biodro2.png" },
        { SoldierPartType::Team2Klata, "gostek-gfx/team2/klata.png" },
        { SoldierPartType::Team2Klata2, "gostek-gfx/team2/klata2.png" },
        { SoldierPartType::Team2Morda, "gostek-gfx/team2/morda.png" },
        { SoldierPartType::Team2Morda2, "gostek-gfx/team2/morda2.png" },
        { SoldierPartType::Team2Ramie, "gostek-gfx/team2/ramie.png" },
        { SoldierPartType::Team2Ramie2, "gostek-gfx/team2/ramie2.png" },
        { SoldierPartType::Team2Reka, "gostek-gfx/team2/reka.png" },
        { SoldierPartType::Team2Reka2, "gostek-gfx/team2/reka2.png" },
        { SoldierPartType::Team2Dlon, "gostek-gfx/team2/dlon.png" },
        { SoldierPartType::Team2Dlon2, "gostek-gfx/team2/dlon2.png" },
        { SoldierPartType::Team2Lancuch, "gostek-gfx/team2/lancuch.png" },
        { SoldierPartType::Team2Helm, "gostek-gfx/team2/helm.png" },
        { SoldierPartType::Team2Helm2, "gostek-gfx/team2/helm2.png" },
        { SoldierPartType::Team2Badge, "gostek-gfx/team2/badge.png" },
        { SoldierPartType::Team2Badge2, "gostek-gfx/team2/badge2.png" },
        { SoldierPartType::Team2Cygaro, "gostek-gfx/team2/cygaro.png" },
        { SoldierPartType::Team2Cygaro2, "gostek-gfx/team2/cygaro2.png" },
        { SoldierPartType::Team2Metal, "gostek-gfx/team2/metal.png" },
        { SoldierPartType::Team2Metal2, "gostek-gfx/team2/metal2.png" },
        { SoldierPartType::Team2Zloto, "gostek-gfx/team2/zloto.png" },
        { SoldierPartType::Team2Zloto2, "gostek-gfx/team2/zloto2.png" },
        { SoldierPartType::Team2Zlotylancuch, "gostek-gfx/team2/zlotylancuch.png" },
        { SoldierPartType::Team2Kap, "gostek-gfx/team2/kap.png" },
        { SoldierPartType::Team2Kap2, "gostek-gfx/team2/kap2.png" },
        { SoldierPartType::Team2Dred, "gostek-gfx/team2/dred.png" },
        { SoldierPartType::Team2Hair1, "gostek-gfx/team2/hair1.png" },
        { SoldierPartType::Team2Hair12, "gostek-gfx/team2/hair12.png" },
        { SoldierPartType::Team2Hair2, "gostek-gfx/team2/hair2.png" },
        { SoldierPartType::Team2Hair22, "gostek-gfx/team2/hair22.png" },
        { SoldierPartType::Team2Hair3, "gostek-gfx/team2/hair3.png" },
        { SoldierPartType::Team2Hair32, "gostek-gfx/team2/hair32.png" },
        { SoldierPartType::Team2Hair4, "gostek-gfx/team2/hair4.png" },
        { SoldierPartType::Team2Hair42, "gostek-gfx/team2/hair42.png" },
        { SoldierPartType::Team2Kamizelka, "gostek-gfx/team2/kamizelka.png" },
        { SoldierPartType::Team2Kamizelka2, "gostek-gfx/team2/kamizelka2.png" },
        { SoldierPartType::Team2RannyKlata, "gostek-gfx/team2/ranny/klata.png" },
        { SoldierPartType::Team2RannyKlata2, "gostek-gfx/team2/ranny/klata2.png" },
        { SoldierPartType::Team2RannyMorda, "gostek-gfx/team2/ranny/morda.png" },
        { SoldierPartType::Team2RannyMorda2, "gostek-gfx/team2/ranny/morda2.png" },
        { SoldierPartType::Team2RannyNoga, "gostek-gfx/team2/ranny/noga.png" },
        { SoldierPartType::Team2RannyNoga2, "gostek-gfx/team2/ranny/noga2.png" },
        { SoldierPartType::Team2RannyRamie, "gostek-gfx/team2/ranny/ramie.png" },
        { SoldierPartType::Team2RannyRamie2, "gostek-gfx/team2/ranny/ramie2.png" },
        { SoldierPartType::Team2RannyReka, "gostek-gfx/team2/ranny/reka.png" },
        { SoldierPartType::Team2RannyReka2, "gostek-gfx/team2/ranny/reka2.png" },
        { SoldierPartType::Team2RannyUdo, "gostek-gfx/team2/ranny/udo.png" },
        { SoldierPartType::Team2RannyUdo2, "gostek-gfx/team2/ranny/udo2.png" },
        { SoldierPartType::Team2RannyBiodro, "gostek-gfx/team2/ranny/biodro.png" },
        { SoldierPartType::Team2RannyBiodro2, "gostek-gfx/team2/ranny/biodro2.png" },
        { SoldierPartType::Team2Lecistopa, "gostek-gfx/team2/lecistopa.png" },
        { SoldierPartType::Team2Lecistopa2, "gostek-gfx/team2/lecistopa2.png" },

        // Preserve order of these parachute textures.

        { SoldierPartType::ParaRope, "gostek-gfx/para-rope.png" },
        { SoldierPartType::Para, "gostek-gfx/para.png" },
        { SoldierPartType::Para2, "gostek-gfx/para2.png" },

        { WeaponType::Shell, "weapons-gfx/shell.png" },
        { WeaponType::Bullet, "weapons-gfx/bullet.png" },
        { WeaponType::Smudge, "weapons-gfx/smudge.png" },
        { WeaponType::Missile, "weapons-gfx/missile.png" },
        { WeaponType::Cluster, "weapons-gfx/cluster.png" },
        { WeaponType::ClusterGrenade, "weapons-gfx/cluster-grenade.png" },
        { WeaponType::FragGrenade, "weapons-gfx/frag-grenade.png" },
        { WeaponType::Ak74, "weapons-gfx/ak74.png" },
        { WeaponType::Ak742, "weapons-gfx/ak74-2.png" },
        { WeaponType::Ak74Clip, "weapons-gfx/ak74-clip.png" },
        { WeaponType::Ak74Clip2, "weapons-gfx/ak74-clip2.png" },
        { WeaponType::Ak74Shell, "weapons-gfx/ak74-shell.png" },
        { WeaponType::Ak74Bullet, "weapons-gfx/ak74-bullet.png" },
        { WeaponType::Ak74Fire, "weapons-gfx/ak74-fire.png" },
        { WeaponType::Minimi, "weapons-gfx/m249.png" },
        { WeaponType::Minimi2, "weapons-gfx/m249-2.png" },
        { WeaponType::MinimiClip, "weapons-gfx/m249-clip.png" },
        { WeaponType::MinimiClip2, "weapons-gfx/m249-clip2.png" },
        { WeaponType::MinimiShell, "weapons-gfx/m249-shell.png" },
        { WeaponType::MinimiBullet, "weapons-gfx/m249-bullet.png" },
        { WeaponType::MinimiFire, "weapons-gfx/m249-fire.png" },
        { WeaponType::Ruger, "weapons-gfx/ruger77.png" },
        { WeaponType::Ruger2, "weapons-gfx/ruger77-2.png" },
        { WeaponType::RugerShell, "weapons-gfx/ruger77-shell.png" },
        { WeaponType::RugerBullet, "weapons-gfx/ruger77-bullet.png" },
        { WeaponType::RugerFire, "weapons-gfx/ruger77-fire.png" },
        { WeaponType::Mp5, "weapons-gfx/mp5.png" },
        { WeaponType::Mp52, "weapons-gfx/mp5-2.png" },
        { WeaponType::Mp5Clip, "weapons-gfx/mp5-clip.png" },
        { WeaponType::Mp5Clip2, "weapons-gfx/mp5-clip2.png" },
        { WeaponType::Mp5Shell, "weapons-gfx/mp5-shell.png" },
        { WeaponType::Mp5Bullet, "weapons-gfx/mp5-bullet.png" },
        { WeaponType::Mp5Fire, "weapons-gfx/mp5-fire.png" },
        { WeaponType::Spas, "weapons-gfx/spas12.png" },
        { WeaponType::Spas2, "weapons-gfx/spas12-2.png" },
        { WeaponType::SpasShell, "weapons-gfx/spas12-shell.png" },
        { WeaponType::SpasBullet, "weapons-gfx/spas12-bullet.png" },
        { WeaponType::SpasFire, "weapons-gfx/spas12-fire.png" },
        { WeaponType::M79, "weapons-gfx/m79.png" },
        { WeaponType::M792, "weapons-gfx/m79-2.png" },
        { WeaponType::M79Clip, "weapons-gfx/m79-clip.png" },
        { WeaponType::M79Clip2, "weapons-gfx/m79-clip2.png" },
        { WeaponType::M79Shell, "weapons-gfx/m79-shell.png" },
        { WeaponType::M79Bullet, "weapons-gfx/m79-bullet.png" },
        { WeaponType::M79Fire, "weapons-gfx/m79-fire.png" },
        { WeaponType::Deagles, "weapons-gfx/deserteagle.png" },
        { WeaponType::Deagles2, "weapons-gfx/deserteagle-2.png" },
        { WeaponType::NDeagles, "weapons-gfx/n-deserteagle.png" },
        { WeaponType::NDeagles2, "weapons-gfx/n-deserteagle-2.png" },
        { WeaponType::DeaglesClip, "weapons-gfx/deserteagle-clip.png" },
        { WeaponType::DeaglesClip2, "weapons-gfx/deserteagle-clip2.png" },
        { WeaponType::DeaglesShell, "weapons-gfx/eagles-shell.png" },
        { WeaponType::DeaglesBullet, "weapons-gfx/eagles-bullet.png" },
        { WeaponType::DeaglesFire, "weapons-gfx/eagles-fire.png" },
        { WeaponType::Steyr, "weapons-gfx/steyraug.png" },
        { WeaponType::Steyr2, "weapons-gfx/steyraug-2.png" },
        { WeaponType::SteyrClip, "weapons-gfx/steyraug-clip.png" },
        { WeaponType::SteyrClip2, "weapons-gfx/steyraug-clip2.png" },
        { WeaponType::SteyrShell, "weapons-gfx/steyraug-shell.png" },
        { WeaponType::SteyrBullet, "weapons-gfx/steyraug-bullet.png" },
        { WeaponType::SteyrFire, "weapons-gfx/steyraug-fire.png" },
        { WeaponType::Barrett, "weapons-gfx/barretm82.png" },
        { WeaponType::Barrett2, "weapons-gfx/barretm82-2.png" },
        { WeaponType::BarrettClip, "weapons-gfx/barretm82-clip.png" },
        { WeaponType::BarrettClip2, "weapons-gfx/barretm82-clip2.png" },
        { WeaponType::BarrettShell, "weapons-gfx/barretm82-shell.png" },
        { WeaponType::BarrettBullet, "weapons-gfx/barretm82-bullet.png" },
        { WeaponType::BarrettFire, "weapons-gfx/barret-fire.png" },
        { WeaponType::Minigun, "weapons-gfx/minigun.png" },
        { WeaponType::Minigun2, "weapons-gfx/minigun-2.png" },
        { WeaponType::MinigunClip, "weapons-gfx/minigun-clip.png" },
        { WeaponType::MinigunShell, "weapons-gfx/minigun-shell.png" },
        { WeaponType::MinigunBullet, "weapons-gfx/minigun-bullet.png" },
        { WeaponType::MinigunFire, "weapons-gfx/minigun-fire.png" },
        { WeaponType::Socom, "weapons-gfx/colt1911.png" },
        { WeaponType::Socom2, "weapons-gfx/colt1911-2.png" },
        { WeaponType::NSocom, "weapons-gfx/n-colt1911.png" },
        { WeaponType::NSocom2, "weapons-gfx/n-colt1911-2.png" },
        { WeaponType::SocomClip, "weapons-gfx/colt1911-clip.png" },
        { WeaponType::SocomClip2, "weapons-gfx/colt1911-clip2.png" },
        { WeaponType::ColtShell, "weapons-gfx/colt-shell.png" },
        { WeaponType::ColtBullet, "weapons-gfx/colt-bullet.png" },
        { WeaponType::SocomFire, "weapons-gfx/colt1911-fire.png" },
        { WeaponType::Bow, "weapons-gfx/bow.png" },
        { WeaponType::Bow2, "weapons-gfx/bow-2.png" },
        { WeaponType::BowS, "weapons-gfx/bow-s.png" },
        { WeaponType::BowA, "weapons-gfx/bow-a.png" },
        { WeaponType::NBow, "weapons-gfx/n-bow.png" },
        { WeaponType::NBow2, "weapons-gfx/n-bow-2.png" },
        { WeaponType::Arrow, "weapons-gfx/arrow.png" },
        { WeaponType::BowFire, "weapons-gfx/bow-fire.png" },
        { WeaponType::Flamer, "weapons-gfx/flamer.png" },
        { WeaponType::Flamer2, "weapons-gfx/flamer-2.png" },
        { WeaponType::FlamerFire, "weapons-gfx/flamer-fire.png" },
        { WeaponType::Knife, "weapons-gfx/knife.png" },
        { WeaponType::Knife2, "weapons-gfx/knife2.png" },
        { WeaponType::Chainsaw, "weapons-gfx/chainsaw.png" },
        { WeaponType::Chainsaw2, "weapons-gfx/chainsaw2.png" },
        { WeaponType::ChainsawFire, "weapons-gfx/chainsaw-fire.png" },
        { WeaponType::Law, "weapons-gfx/law.png" },
        { WeaponType::Law2, "weapons-gfx/law-2.png" },
        { WeaponType::LawFire, "weapons-gfx/law-fire.png" },
        { WeaponType::M2, "weapons-gfx/m2.png" },
        { WeaponType::M22, "weapons-gfx/m2-2.png" },
        { WeaponType::M2Stat, "weapons-gfx/m2-stat.png" },
    };

    std::ranges::for_each(
      std::as_const(all_sprite_file_paths), [&](const auto& type_and_file_path) {
          all_sprites_.insert(
            { type_and_file_path.first, Texture::Load(type_and_file_path.second.c_str()) });
      });

    // clang-format off
    soldier_part_type_to_data_.emplace_back(SoldierPartSecondaryWeaponType::Deagles, nullptr);
    AddSprite(SoldierPartSecondaryWeaponType::Mp5, WeaponType::Mp5, WeaponType::Mp52, {5, 10}, {0.300, 0.300}, false, false, 0.0F, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartSecondaryWeaponType::Ak74, WeaponType::Ak74, WeaponType::Ak742, {5, 10}, {0.300, 0.250}, false, false, 0.0F, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartSecondaryWeaponType::Steyr, WeaponType::Steyr, WeaponType::Steyr2, {5, 10}, {0.300, 0.500}, false, false, 0.0F, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartSecondaryWeaponType::Spas, WeaponType::Spas, WeaponType::Spas2, {5, 10}, {0.300, 0.300}, false, false, 0.0F, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartSecondaryWeaponType::Ruger, WeaponType::Ruger, WeaponType::Ruger2, {5, 10}, {0.300, 0.300}, false, false, 0.0F, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartSecondaryWeaponType::M79, WeaponType::M79, WeaponType::M792, {5, 10}, {0.300, 0.350}, false, false, 0.0F, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartSecondaryWeaponType::Barrett, WeaponType::Barrett, WeaponType::Barrett2, {5, 10}, {0.300, 0.350}, false, false, 0.0F, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartSecondaryWeaponType::Minimi, WeaponType::Minimi, WeaponType::Minimi2, {5, 10}, {0.300, 0.350}, false, false, 0.0F, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartSecondaryWeaponType::Minigun, WeaponType::Minigun, WeaponType::Minigun2, {5, 10}, {0.200, 0.500}, false, false, 0.0F, SoldierColor::None, SoldierAlpha::Base);
    soldier_part_type_to_data_.emplace_back(SoldierPartSecondaryWeaponType::Socom, nullptr);
    soldier_part_type_to_data_.emplace_back(SoldierPartSecondaryWeaponType::Knife, nullptr);
    AddSprite(SoldierPartSecondaryWeaponType::Chainsaw, WeaponType::Chainsaw, WeaponType::Chainsaw2, {5, 10}, {0.250, 0.500}, false, false, 0.0F, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartSecondaryWeaponType::Law, WeaponType::Law, WeaponType::Law2, {5, 10}, {0.300, 0.450}, false, false, 0.0F, SoldierColor::None, SoldierAlpha::Base);
    soldier_part_type_to_data_.emplace_back(SoldierPartSecondaryWeaponType::Flamebow, nullptr);
    soldier_part_type_to_data_.emplace_back(SoldierPartSecondaryWeaponType::Bow, nullptr);
    AddSprite(SoldierPartSecondaryWeaponType::Flamer, WeaponType::Flamer, WeaponType::Flamer2, {5, 10}, {0.300, 0.300}, false, false, 0.0F, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Udo, SoldierPartType::Udo, SoldierPartType::Udo2, {6, 3}, {0.200, 0.500}, true, true, 5.0, SoldierColor::Pants, SoldierAlpha::Base);
    AddSprite(SoldierPartType::RannyUdo, SoldierPartType::Udo, SoldierPartType::Udo2, {6, 3}, {0.200, 0.500}, false, true, 5.0, SoldierColor::None, SoldierAlpha::Blood);
    AddSprite(SoldierPartType::Stopa, SoldierPartType::Stopa, SoldierPartType::Stopa2, {2, 18}, {0.350, 0.350}, true, true, 0.0, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Lecistopa, SoldierPartType::Lecistopa, SoldierPartType::Lecistopa2, {2, 18}, {0.350, 0.350}, false, true, 0.0, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Noga, SoldierPartType::Noga, SoldierPartType::Noga2, {3, 2}, {0.150, 0.550}, true, true, 0.0, SoldierColor::Pants, SoldierAlpha::Base);
    AddSprite(SoldierPartType::RannyNoga, SoldierPartType::Noga, SoldierPartType::Noga2, {3, 2}, {0.150, 0.550}, false, true, 0.0, SoldierColor::None, SoldierAlpha::Blood);
    AddSprite(SoldierPartType::Ramie, SoldierPartType::Ramie, SoldierPartType::Ramie2, {11, 14}, {0.000, 0.500}, true, true, 0.0, SoldierColor::Main, SoldierAlpha::Base);
    AddSprite(SoldierPartType::RannyRamie, SoldierPartType::Ramie, SoldierPartType::Ramie2, {11, 14}, {0.000, 0.500}, false, true, 0.0, SoldierColor::None, SoldierAlpha::Blood);
    AddSprite(SoldierPartType::Reka, SoldierPartType::Reka, std::nullopt, {14, 15}, {0.000, 0.500}, true, true, 5.0, SoldierColor::Main, SoldierAlpha::Base);
    AddSprite(SoldierPartType::RannyReka, SoldierPartType::Reka, SoldierPartType::Reka2, {14, 15}, {0.000, 0.500}, false, true, 5.0, SoldierColor::None, SoldierAlpha::Blood);
    AddSprite(SoldierPartType::Dlon, SoldierPartType::Dlon, SoldierPartType::Dlon2, {15, 19}, {0.000, 0.400}, true, true, 0.0, SoldierColor::Skin, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Helm, SoldierPartType::Helm, SoldierPartType::Helm2, {15, 19}, {0.000, 0.500}, false, true, 0.0, SoldierColor::Main, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Kap, SoldierPartType::Kap, SoldierPartType::Kap2, {15, 19}, {0.100, 0.400}, false, true, 0.0, SoldierColor::Main, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Udo, SoldierPartType::Udo, SoldierPartType::Udo2, {5, 4}, {0.200, 0.650}, true, true, 5.0, SoldierColor::Pants, SoldierAlpha::Base);
    AddSprite(SoldierPartType::RannyUdo, SoldierPartType::Udo, SoldierPartType::Udo2, {5, 4}, {0.200, 0.650}, false, true, 5.0, SoldierColor::None, SoldierAlpha::Blood);
    AddSprite(SoldierPartType::Stopa, SoldierPartType::Stopa, SoldierPartType::Stopa2, {1, 17}, {0.350, 0.350}, true, true, 0.0, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Lecistopa, SoldierPartType::Lecistopa, SoldierPartType::Lecistopa2, {1, 17}, {0.350, 0.350}, false, true, 0.0, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Noga, SoldierPartType::Noga, SoldierPartType::Noga2, {4, 1}, {0.150, 0.550}, true, true, 0.0, SoldierColor::Pants, SoldierAlpha::Base);
    AddSprite(SoldierPartType::RannyNoga, SoldierPartType::Noga, SoldierPartType::Noga2, {4, 1}, {0.150, 0.550}, false, true, 0.0, SoldierColor::None, SoldierAlpha::Blood);
    AddSprite(SoldierPartType::Klata, SoldierPartType::Klata, SoldierPartType::Klata2, {10, 11}, {0.100, 0.300}, true, true, 0.0, SoldierColor::Main, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Kamizelka, SoldierPartType::Kamizelka, SoldierPartType::Kamizelka2, {10, 11}, {0.100, 0.300}, false, true, 0.0, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartType::RannyKlata, SoldierPartType::Klata, SoldierPartType::Klata2, {10, 11}, {0.100, 0.300}, false, true, 0.0, SoldierColor::None, SoldierAlpha::Blood);
    AddSprite(SoldierPartType::Biodro, SoldierPartType::Biodro, SoldierPartType::Biodro2, {5, 6}, {0.250, 0.600}, true, true, 0.0, SoldierColor::Main, SoldierAlpha::Base);
    AddSprite(SoldierPartType::RannyBiodro, SoldierPartType::Biodro, SoldierPartType::Biodro2, {5, 6}, {0.250, 0.600}, false, true, 0.0, SoldierColor::None, SoldierAlpha::Blood);
    AddSprite(SoldierPartType::Morda, SoldierPartType::Morda, SoldierPartType::Morda2, {9, 12}, {0.000, 0.500}, true, true, 0.0, SoldierColor::Skin, SoldierAlpha::Base);
    AddSprite(SoldierPartType::RannyMorda, SoldierPartType::Morda, SoldierPartType::Morda2, {9, 12}, {0.000, 0.500}, false, true, 0.0, SoldierColor::Headblood, SoldierAlpha::Blood);
    AddSprite(SoldierPartType::Morda, SoldierPartType::Morda, SoldierPartType::Morda2, {9, 12}, {0.500, 0.500}, false, true, 0.0, SoldierColor::Skin, SoldierAlpha::Base);
    AddSprite(SoldierPartType::RannyMorda, SoldierPartType::Morda, SoldierPartType::Morda2, {9, 12}, {0.500, 0.500}, false, true, 0.0, SoldierColor::Headblood, SoldierAlpha::Blood);
    AddSprite(SoldierPartType::Hair3, SoldierPartType::Hair3, SoldierPartType::Hair32, {9, 12}, {0.000, 0.500}, false, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Helm, SoldierPartType::Helm, SoldierPartType::Helm2, {9, 12}, {0.000, 0.500}, false, true, 0.0, SoldierColor::Main, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Kap, SoldierPartType::Kap, SoldierPartType::Kap2, {9, 12}, {0.000, 0.500}, false, true, 0.0, SoldierColor::Main, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Badge, SoldierPartType::Badge, SoldierPartType::Badge2, {9, 12}, {0.000, 0.500}, false, true, 0.0, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Hair1, SoldierPartType::Hair1, SoldierPartType::Hair12, {9, 12}, {0.000, 0.500}, false, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Dred, SoldierPartType::Dred, std::nullopt, {23, 24}, {0.000, 1.220}, false, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Dred, SoldierPartType::Dred, std::nullopt, {23, 24}, {0.100, 0.500}, false, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Dred, SoldierPartType::Dred, std::nullopt, {23, 24}, {0.040, -0.300}, false, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Dred, SoldierPartType::Dred, std::nullopt, {23, 24}, {0.000, -0.900}, false, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Dred, SoldierPartType::Dred, std::nullopt, {23, 24}, {-0.200, -1.350}, false, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Hair2, SoldierPartType::Hair2, SoldierPartType::Hair22, {9, 12}, {0.000, 0.500}, false, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Hair4, SoldierPartType::Hair4, SoldierPartType::Hair42, {9, 12}, {0.000, 0.500}, false, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Cygaro, SoldierPartType::Cygaro, SoldierPartType::Cygaro2, {9, 12}, {-0.125, 0.400}, false, true, 0.0, SoldierColor::Cygar, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Lancuch, SoldierPartType::Lancuch, std::nullopt, {10, 22}, {0.100, 0.500}, false, true, 0.0, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Lancuch, SoldierPartType::Lancuch, std::nullopt, {11, 22}, {0.100, 0.500}, false, true, 0.0, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Metal, SoldierPartType::Metal, SoldierPartType::Metal2, {22, 21}, {0.500, 0.700}, false, true, 0.0, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Zlotylancuch, SoldierPartType::Zlotylancuch, std::nullopt, {10, 22}, {0.100, 0.500}, false, true, 0.0, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Zlotylancuch, SoldierPartType::Zlotylancuch, std::nullopt, {11, 22}, {0.100, 0.500}, false, true, 0.0, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartType::Zloto, SoldierPartType::Zloto, SoldierPartType::Zloto2, {22, 21}, {0.500, 0.500}, false, true, 0.0, SoldierColor::None, SoldierAlpha::Base);
    AddSprite(SoldierPartTertiaryWeaponType::FragGrenade1, WeaponType::FragGrenade, std::nullopt, { 5,  6}, { 0.500,  0.100}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Nades);
    AddSprite(SoldierPartTertiaryWeaponType::FragGrenade2, WeaponType::FragGrenade, std::nullopt, { 5,  6}, { 0.500,  0.100}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Nades);
    AddSprite(SoldierPartTertiaryWeaponType::FragGrenade3, WeaponType::FragGrenade, std::nullopt, { 5,  6}, { 0.500,  0.100}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Nades);
    AddSprite(SoldierPartTertiaryWeaponType::FragGrenade4, WeaponType::FragGrenade, std::nullopt, { 5,  6}, { 0.500,  0.100}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Nades);
    AddSprite(SoldierPartTertiaryWeaponType::FragGrenade5, WeaponType::FragGrenade, std::nullopt, { 5,  6}, { 0.500,  0.100}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Nades);
    AddSprite(SoldierPartTertiaryWeaponType::ClusterGrenade1, WeaponType::ClusterGrenade, std::nullopt, { 5,  6}, { 0.500,  0.300}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Nades);
    AddSprite(SoldierPartTertiaryWeaponType::ClusterGrenade2, WeaponType::ClusterGrenade, std::nullopt, { 5,  6}, { 0.500,  0.300}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Nades);
    AddSprite(SoldierPartTertiaryWeaponType::ClusterGrenade3, WeaponType::ClusterGrenade, std::nullopt, { 5,  6}, { 0.500,  0.300}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Nades);
    AddSprite(SoldierPartTertiaryWeaponType::ClusterGrenade4, WeaponType::ClusterGrenade, std::nullopt, { 5,  6}, { 0.500,  0.300}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Nades);
    AddSprite(SoldierPartTertiaryWeaponType::ClusterGrenade5, WeaponType::ClusterGrenade, std::nullopt, { 5,  6}, { 0.500,  0.300}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Nades);
    AddSprite(SoldierPartPrimaryWeaponType::Deagles, WeaponType::Deagles, WeaponType::Deagles2, {16, 15}, { 0.100,  0.800}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::DeaglesClip, WeaponType::DeaglesClip, WeaponType::DeaglesClip2, {16, 15}, { 0.100,  0.800}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::DeaglesFire, WeaponType::DeaglesFire, std::nullopt, {16, 15}, {-0.500,  1.000}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::Mp5, WeaponType::Mp5, WeaponType::Mp52, {16, 15}, { 0.150,  0.600}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::Mp5Clip, WeaponType::Mp5Clip, WeaponType::Mp5Clip2, {16, 15}, { 0.150,  0.600}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::Mp5Fire, WeaponType::Mp5Fire, std::nullopt, {16, 15}, { -0.65,  0.850}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::Ak74, WeaponType::Ak74, WeaponType::Ak742, {16, 15}, { 0.150,  0.500}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::Ak74Clip, WeaponType::Ak74Clip, WeaponType::Ak74Clip2, {16, 15}, { 0.150,  0.500}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::Ak74Fire, WeaponType::Ak74Fire, std::nullopt, {16, 15}, { -0.37,  0.800}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::Steyr, WeaponType::Steyr, WeaponType::Steyr2, {16, 15}, { 0.200,  0.600}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::SteyrClip, WeaponType::SteyrClip, WeaponType::SteyrClip2, {16, 15}, { 0.200,  0.600}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::SteyrFire, WeaponType::SteyrFire, std::nullopt, {16, 15}, { -0.24,  0.750}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::Spas, WeaponType::Spas, WeaponType::Spas2, {16, 15}, { 0.100,  0.600}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    soldier_part_type_to_data_.emplace_back(SoldierPartPrimaryWeaponType::SpasClip, nullptr);
    AddSprite(SoldierPartPrimaryWeaponType::SpasFire, WeaponType::SpasFire, std::nullopt, {16, 15}, {-0.200,  0.900}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::Ruger, WeaponType::Ruger, WeaponType::Ruger2, {16, 15}, { 0.100,  0.700}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    soldier_part_type_to_data_.emplace_back(SoldierPartPrimaryWeaponType::RugerClip, nullptr);
    AddSprite(SoldierPartPrimaryWeaponType::RugerFire, WeaponType::RugerFire, std::nullopt, {16, 15}, { -0.35,  0.850}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::M79, WeaponType::M79, WeaponType::M792, {16, 15}, { 0.100,  0.700}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::M79Clip, WeaponType::M79Clip, WeaponType::M79Clip2, {16, 15}, { 0.100,  0.700}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::M79Fire, WeaponType::M79Fire, std::nullopt, {16, 15}, {-0.400,  0.800}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::Barrett, WeaponType::Barrett, WeaponType::Barrett2, {16, 15}, { 0.150,  0.700}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::BarrettClip, WeaponType::BarrettClip, WeaponType::BarrettClip2, {16, 15}, { 0.150,  0.700}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::BarrettFire, WeaponType::BarrettFire, std::nullopt, {16, 15}, { -0.15,  0.800}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::Minimi, WeaponType::Minimi, WeaponType::Minimi2, {16, 15}, { 0.150,  0.600}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::MinimiClip, WeaponType::MinimiClip, WeaponType::MinimiClip2, {16, 15}, { 0.150,  0.600}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::MinimiFire, WeaponType::MinimiFire, std::nullopt, {16, 15}, {-0.200,  0.900}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::MinigunClip, WeaponType::MinigunClip, std::nullopt, { 8,  7}, { 0.500,  0.100}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::Minigun, WeaponType::Minigun, WeaponType::Minigun2, {16, 15}, { 0.050,  0.500}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::MinigunFire, WeaponType::MinigunFire, std::nullopt, {16, 15}, {-0.200,  0.450}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::Socom, WeaponType::Socom, WeaponType::Socom2, {16, 15}, { 0.200,  0.550}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::SocomClip, WeaponType::SocomClip, WeaponType::SocomClip2, {16, 15}, { 0.200,  0.550}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::SocomFire, WeaponType::SocomFire, std::nullopt, {16, 15}, { -0.24,  0.850}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::Knife, WeaponType::Knife, WeaponType::Knife2, {16, 20}, {-0.100,  0.600}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    soldier_part_type_to_data_.emplace_back(SoldierPartPrimaryWeaponType::KnifeClip, nullptr);
    soldier_part_type_to_data_.emplace_back(SoldierPartPrimaryWeaponType::KnifeFire, nullptr);
    AddSprite(SoldierPartPrimaryWeaponType::Chainsaw, WeaponType::Chainsaw, WeaponType::Chainsaw2, {16, 15}, { 0.100,  0.500}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    soldier_part_type_to_data_.emplace_back(SoldierPartPrimaryWeaponType::ChainsawClip, nullptr);
    AddSprite(SoldierPartPrimaryWeaponType::ChainsawFire, WeaponType::ChainsawFire, std::nullopt, {16, 15}, { 0.000,  0.000}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::Law, WeaponType::Law, WeaponType::Law2, {16, 15}, { 0.100,  0.600}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    soldier_part_type_to_data_.emplace_back(SoldierPartPrimaryWeaponType::LawClip, nullptr);
    AddSprite(SoldierPartPrimaryWeaponType::LawFire, WeaponType::LawFire, std::nullopt, {16, 15}, {-0.100,  0.550}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::Bow, WeaponType::Bow, WeaponType::Bow2, {16, 15}, {-0.400,  0.550}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::BowArrow, WeaponType::BowA, std::nullopt, {16, 15}, { 0.000,  0.550}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::BowString, WeaponType::BowS, std::nullopt, {16, 15}, {-0.400,  0.550}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::BowReload, WeaponType::Bow, WeaponType::Bow2, {16, 15}, {-0.400,  0.550}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::BowArrowReload, WeaponType::Arrow, std::nullopt, {16, 20}, { 0.000,  0.550}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::BowStringReload, WeaponType::BowS, std::nullopt, {16, 15}, {-0.400,  0.550}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::BowFire, WeaponType::BowFire, std::nullopt, {16, 15}, { 0.000,  0.000}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartPrimaryWeaponType::Flamer, WeaponType::Flamer, WeaponType::Flamer2, {16, 15}, { 0.200,  0.700}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    soldier_part_type_to_data_.emplace_back(SoldierPartPrimaryWeaponType::FlamerClip, nullptr);
    AddSprite(SoldierPartPrimaryWeaponType::FlamerFire, WeaponType::FlamerFire, std::nullopt, {16, 15}, { 0.000,  0.000}, false, false, 0.0, SoldierColor::None, SoldierAlpha::Base );
    AddSprite(SoldierPartType::Ramie, SoldierPartType::Ramie, SoldierPartType::Ramie2, {10, 13}, {0.000, 0.600}, true, true, 0.0, SoldierColor::Main, SoldierAlpha::Base);
    AddSprite(SoldierPartType::RannyRamie, SoldierPartType::Ramie, SoldierPartType::Ramie2, {10, 13}, {-0.100, 0.500}, false, true, 0.0, SoldierColor::None, SoldierAlpha::Blood);
    AddSprite(SoldierPartType::Reka, SoldierPartType::Reka, std::nullopt, {13, 16}, {0.000, 0.600}, true, true, 5.0, SoldierColor::Main, SoldierAlpha::Base);
    AddSprite(SoldierPartType::RannyReka, SoldierPartType::Reka, SoldierPartType::Reka2, {13, 16}, {0.000, 0.600}, false, true, 5.0, SoldierColor::None, SoldierAlpha::Blood);
    AddSprite(SoldierPartType::Dlon, SoldierPartType::Dlon, SoldierPartType::Dlon2, {16, 20}, {0.000, 0.500}, true, true, 0.0, SoldierColor::Skin, SoldierAlpha::Base);
    // clang-format on
    std::cout << "Loaded " << soldier_part_type_to_data_.size() << " soldier part sprites"
              << std::endl;
}

SpriteManager::~SpriteManager()
{
    std::ranges::for_each(std::as_const(all_sprites_), [&](const auto& type_and_texture_data) {
        Texture::Delete(type_and_texture_data.second.opengl_id);
    });
}

void SpriteManager::AddSprite(TSoldierPartSpriteKey soldier_part_sprite_key,
                              TSpriteKey sprite_key,
                              std::optional<TSpriteKey> flipped_sprite_key,
                              glm::uvec2 point,
                              glm::vec2 center,
                              bool visible,
                              bool team,
                              float flexibility,
                              SoldierColor color,
                              SoldierAlpha alpha)
{
    std::optional<Texture::TextureData> flipped_sprite_texture_data = std::nullopt;
    if (flipped_sprite_key) {
        flipped_sprite_texture_data = all_sprites_[*flipped_sprite_key];
    }
    soldier_part_type_to_data_.emplace_back(
      soldier_part_sprite_key,
      std::make_unique<SoldierPartData>(all_sprites_[sprite_key],
                                        point,
                                        center,
                                        visible,
                                        flipped_sprite_texture_data,
                                        team,
                                        flexibility,
                                        color,
                                        alpha));
}

const SoldierPartData* SpriteManager::GetSoldierPartData(unsigned int id) const
{
    return soldier_part_type_to_data_[id].second.get();
}
std::variant<SoldierPartType,
             SoldierPartPrimaryWeaponType,
             SoldierPartSecondaryWeaponType,
             SoldierPartTertiaryWeaponType>
SpriteManager::GetSoldierPartDataType(unsigned int id) const
{
    return soldier_part_type_to_data_[id].first;
}
unsigned int SpriteManager::GetSoldierPartCount() const
{
    return soldier_part_type_to_data_.size();
}
} // namespace Soldat::Sprites
