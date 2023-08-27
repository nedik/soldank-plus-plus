#include "Sprites.hpp"

#include "rendering/data/Texture.hpp"

#include <iostream>
#include <memory>
#include <variant>
#include <vector>

namespace Soldat::Sprites
{
std::unique_ptr<
  std::vector<std::pair<std::variant<SoldierPartType>, std::unique_ptr<SoldierPartData>>>>
  soldier_part_type_to_data;

SoldierPartData::SoldierPartData(const std::filesystem::path& file_path,
                                 glm::uvec2 point,
                                 glm::vec2 center,
                                 bool visible,
                                 bool flip,
                                 bool team,
                                 float flexibility,
                                 SoldierColor color,
                                 SoldierAlpha alpha)
    : file_path_(file_path)
    , point_(point)
    , center_(center)
    , visible_(visible)
    , flip_(flip)
    , team_(team)
    , flexibility_(flexibility)
    , color_(color)
    , alpha_(alpha)
    , texture_width_(0)
    , texture_height_(0)
    , texture_flipped_width_(0)
    , texture_flipped_height_(0)
{
    texture_ = Texture::Load(file_path.string().c_str(), &texture_width_, &texture_height_);
    if (flip_) {
        std::filesystem::path flipped_file_path = file_path;
        std::filesystem::path extension = file_path.extension();
        flipped_file_path.replace_extension("");
        flipped_file_path += "2";
        flipped_file_path += extension;
        std::cout << "Flipped texture: " << flipped_file_path << std::endl;
        texture_flipped_ = Texture::Load(
          flipped_file_path.string().c_str(), &texture_flipped_width_, &texture_flipped_height_);
    }
}

SoldierPartData::~SoldierPartData()
{
    Texture::Delete(texture_);
    if (flip_) {
        Texture::Delete(texture_flipped_);
    }
}

void Init()
{
    std::cout << "Init Sprites" << std::endl;
    // clang-format off
    auto* v =
      new std::vector<std::pair<std::variant<SoldierPartType>, std::unique_ptr<SoldierPartData>>>();
    v->push_back({ SoldierPartType::Udo, std::make_unique<SoldierPartData>("gostek-gfx/udo.png", glm::uvec2(6, 3), glm::vec2(0.200, 0.500), true, true, true, 5.0, SoldierColor::Pants, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyUdo, std::make_unique<SoldierPartData>("gostek-gfx/ranny/udo.png", glm::uvec2(6, 3), glm::vec2(0.200, 0.500), false, true, true, 5.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Stopa, std::make_unique<SoldierPartData>("gostek-gfx/stopa.png", glm::uvec2(2, 18), glm::vec2(0.350, 0.350), true, true, true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Lecistopa, std::make_unique<SoldierPartData>("gostek-gfx/lecistopa.png", glm::uvec2(2, 18), glm::vec2(0.350, 0.350), false, true, true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Noga, std::make_unique<SoldierPartData>("gostek-gfx/noga.png", glm::uvec2(3, 2), glm::vec2(0.150, 0.550), true, true, true, 0.0, SoldierColor::Pants, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyNoga, std::make_unique<SoldierPartData>("gostek-gfx/ranny/noga.png", glm::uvec2(3, 2), glm::vec2(0.150, 0.550), false, true, true, 0.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Ramie, std::make_unique<SoldierPartData>("gostek-gfx/ramie.png", glm::uvec2(11, 14), glm::vec2(0.000, 0.500), true, true, true, 0.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyRamie, std::make_unique<SoldierPartData>("gostek-gfx/ranny/ramie.png", glm::uvec2(11, 14), glm::vec2(0.000, 0.500), false, true, true, 0.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Reka, std::make_unique<SoldierPartData>("gostek-gfx/reka.png", glm::uvec2(14, 15), glm::vec2(0.000, 0.500), true, false, true, 5.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyReka, std::make_unique<SoldierPartData>("gostek-gfx/ranny/reka.png", glm::uvec2(14, 15), glm::vec2(0.000, 0.500), false, true, true, 5.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Dlon, std::make_unique<SoldierPartData>("gostek-gfx/dlon.png", glm::uvec2(15, 19), glm::vec2(0.000, 0.400), true, true, true, 0.0, SoldierColor::Skin, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Helm, std::make_unique<SoldierPartData>("gostek-gfx/helm.png", glm::uvec2(15, 19), glm::vec2(0.000, 0.500), false, true, true, 0.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Kap, std::make_unique<SoldierPartData>("gostek-gfx/kap.png", glm::uvec2(15, 19), glm::vec2(0.100, 0.400), false, true, true, 0.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Udo, std::make_unique<SoldierPartData>("gostek-gfx/udo.png", glm::uvec2(5, 4), glm::vec2(0.200, 0.650), true, true, true, 5.0, SoldierColor::Pants, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyUdo, std::make_unique<SoldierPartData>("gostek-gfx/ranny/udo.png", glm::uvec2(5, 4), glm::vec2(0.200, 0.650), false, true, true, 5.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Stopa, std::make_unique<SoldierPartData>("gostek-gfx/stopa.png", glm::uvec2(1, 17), glm::vec2(0.350, 0.350), true, true, true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Lecistopa, std::make_unique<SoldierPartData>("gostek-gfx/lecistopa.png", glm::uvec2(1, 17), glm::vec2(0.350, 0.350), false, true, true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Noga, std::make_unique<SoldierPartData>("gostek-gfx/noga.png", glm::uvec2(4, 1), glm::vec2(0.150, 0.550), true, true, true, 0.0, SoldierColor::Pants, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyNoga, std::make_unique<SoldierPartData>("gostek-gfx/ranny/noga.png", glm::uvec2(4, 1), glm::vec2(0.150, 0.550), false, true, true, 0.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Klata, std::make_unique<SoldierPartData>("gostek-gfx/klata.png", glm::uvec2(10, 11), glm::vec2(0.100, 0.300), true, true, true, 0.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Kamizelka, std::make_unique<SoldierPartData>("gostek-gfx/kamizelka.png", glm::uvec2(10, 11), glm::vec2(0.100, 0.300), false, true, true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyKlata, std::make_unique<SoldierPartData>("gostek-gfx/ranny/klata.png", glm::uvec2(10, 11), glm::vec2(0.100, 0.300), false, true, true, 0.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Biodro, std::make_unique<SoldierPartData>("gostek-gfx/biodro.png", glm::uvec2(5, 6), glm::vec2(0.250, 0.600), true, true, true, 0.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyBiodro, std::make_unique<SoldierPartData>("gostek-gfx/ranny/biodro.png", glm::uvec2(5, 6), glm::vec2(0.250, 0.600), false, true, true, 0.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Morda, std::make_unique<SoldierPartData>("gostek-gfx/morda.png", glm::uvec2(9, 12), glm::vec2(0.000, 0.500), true, true, true, 0.0, SoldierColor::Skin, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyMorda, std::make_unique<SoldierPartData>("gostek-gfx/ranny/morda.png", glm::uvec2(9, 12), glm::vec2(0.000, 0.500), false, true, true, 0.0, SoldierColor::Headblood, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Morda, std::make_unique<SoldierPartData>("gostek-gfx/morda.png", glm::uvec2(9, 12), glm::vec2(0.500, 0.500), false, true, true, 0.0, SoldierColor::Skin, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyMorda, std::make_unique<SoldierPartData>("gostek-gfx/ranny/morda.png", glm::uvec2(9, 12), glm::vec2(0.500, 0.500), false, true, true, 0.0, SoldierColor::Headblood, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Hair3, std::make_unique<SoldierPartData>("gostek-gfx/hair3.png", glm::uvec2(9, 12), glm::vec2(0.000, 0.500), false, true, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Helm, std::make_unique<SoldierPartData>("gostek-gfx/helm.png", glm::uvec2(9, 12), glm::vec2(0.000, 0.500), false, true, true, 0.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Kap, std::make_unique<SoldierPartData>("gostek-gfx/kap.png", glm::uvec2(9, 12), glm::vec2(0.000, 0.500), false, true, true, 0.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Badge, std::make_unique<SoldierPartData>("gostek-gfx/badge.png", glm::uvec2(9, 12), glm::vec2(0.000, 0.500), false, true, true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Hair1, std::make_unique<SoldierPartData>("gostek-gfx/hair1.png", glm::uvec2(9, 12), glm::vec2(0.000, 0.500), false, true, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Dred, std::make_unique<SoldierPartData>("gostek-gfx/dred.png", glm::uvec2(23, 24), glm::vec2(0.000, 1.220), false, false, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Dred, std::make_unique<SoldierPartData>("gostek-gfx/dred.png", glm::uvec2(23, 24), glm::vec2(0.100, 0.500), false, false, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Dred, std::make_unique<SoldierPartData>("gostek-gfx/dred.png", glm::uvec2(23, 24), glm::vec2(0.040, -0.300), false, false, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Dred, std::make_unique<SoldierPartData>("gostek-gfx/dred.png", glm::uvec2(23, 24), glm::vec2(0.000, -0.900), false, false, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Dred, std::make_unique<SoldierPartData>("gostek-gfx/dred.png", glm::uvec2(23, 24), glm::vec2(-0.200, -1.350), false, false, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Hair2, std::make_unique<SoldierPartData>("gostek-gfx/hair2.png", glm::uvec2(9, 12), glm::vec2(0.000, 0.500), false, true, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Hair4, std::make_unique<SoldierPartData>("gostek-gfx/hair4.png", glm::uvec2(9, 12), glm::vec2(0.000, 0.500), false, true, true, 0.0, SoldierColor::Hair, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Cygaro, std::make_unique<SoldierPartData>("gostek-gfx/cygaro.png", glm::uvec2(9, 12), glm::vec2(-0.125, 0.400), false, true, true, 0.0, SoldierColor::Cygar, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Lancuch, std::make_unique<SoldierPartData>("gostek-gfx/lancuch.png", glm::uvec2(10, 22), glm::vec2(0.100, 0.500), false, false, true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Lancuch, std::make_unique<SoldierPartData>("gostek-gfx/lancuch.png", glm::uvec2(11, 22), glm::vec2(0.100, 0.500), false, false, true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Metal, std::make_unique<SoldierPartData>("gostek-gfx/metal.png", glm::uvec2(22, 21), glm::vec2(0.500, 0.700), false, true, true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Zlotylancuch, std::make_unique<SoldierPartData>("gostek-gfx/zlotylancuch.png", glm::uvec2(10, 22), glm::vec2(0.100, 0.500), false, false, true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Zlotylancuch, std::make_unique<SoldierPartData>("gostek-gfx/zlotylancuch.png", glm::uvec2(11, 22), glm::vec2(0.100, 0.500), false, false, true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Zloto, std::make_unique<SoldierPartData>("gostek-gfx/zloto.png", glm::uvec2(22, 21), glm::vec2(0.500, 0.500), false, true, true, 0.0, SoldierColor::None, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::Ramie, std::make_unique<SoldierPartData>("gostek-gfx/ramie.png", glm::uvec2(10, 13), glm::vec2(0.000, 0.600), true, true, true, 0.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyRamie, std::make_unique<SoldierPartData>("gostek-gfx/ranny/ramie.png", glm::uvec2(10, 13), glm::vec2(-0.100, 0.500), false, true, true, 0.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Reka, std::make_unique<SoldierPartData>("gostek-gfx/reka.png", glm::uvec2(13, 16), glm::vec2(0.000, 0.600), true, false, true, 5.0, SoldierColor::Main, SoldierAlpha::Base) });
    v->push_back({ SoldierPartType::RannyReka, std::make_unique<SoldierPartData>("gostek-gfx/ranny/reka.png", glm::uvec2(13, 16), glm::vec2(0.000, 0.600), false, true, true, 5.0, SoldierColor::None, SoldierAlpha::Blood) });
    v->push_back({ SoldierPartType::Dlon, std::make_unique<SoldierPartData>("gostek-gfx/dlon.png", glm::uvec2(16, 20), glm::vec2(0.000, 0.500), true, true, true, 0.0, SoldierColor::Skin, SoldierAlpha::Base) });
    soldier_part_type_to_data = std::unique_ptr<
      std::vector<std::pair<std::variant<SoldierPartType>, std::unique_ptr<SoldierPartData>>>>(v);
    // clang-format on
    std::cout << "Loaded " << soldier_part_type_to_data->size() << " gostek-gfx assets"
              << std::endl;
}

const SoldierPartData& Get(unsigned int id)
{
    return *(*soldier_part_type_to_data)[id].second;
}

unsigned int GetSoldierPartCount()
{
    return soldier_part_type_to_data->size();
}

SoldierPartType GetType(unsigned int id)
{
    return std::get<SoldierPartType>((*soldier_part_type_to_data)[id].first);
}

void Free()
{
    soldier_part_type_to_data.reset(nullptr);
    soldier_part_type_to_data.reset(nullptr);
}
} // namespace Soldat::Sprites
