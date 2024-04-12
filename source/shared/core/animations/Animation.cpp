#include "Animation.hpp"

#include "core/utility/Getline.hpp"

#include "spdlog/spdlog.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <exception>
#include <cmath>
#include <array>
#include <utility>

namespace Soldank
{
const AnimationData& Animations::Get(AnimationType animation_type)
{
    static std::array<AnimationData, 44> animations{
        AnimationData{ AnimationType::Stand, "stoi.poa", 3, true },
        AnimationData{ AnimationType::Run, "biega.poa", 1, true },
        AnimationData{ AnimationType::RunBack, "biegatyl.poa", 1, true },
        AnimationData{ AnimationType::Jump, "skok.poa", 1, false },
        AnimationData{ AnimationType::JumpSide, "skokwbok.poa", 1, false },
        AnimationData{ AnimationType::Fall, "spada.poa", 1, false },
        AnimationData{ AnimationType::Crouch, "kuca.poa", 1, false },
        AnimationData{ AnimationType::CrouchRun, "kucaidzie.poa", 2, true },
        AnimationData{ AnimationType::Reload, "laduje.poa", 2, false },
        AnimationData{ AnimationType::Throw, "rzuca.poa", 1, false },
        AnimationData{ AnimationType::Recoil, "odrzut.poa", 1, false },
        AnimationData{ AnimationType::SmallRecoil, "odrzut2.poa", 1, false },
        AnimationData{ AnimationType::Shotgun, "shotgun.poa", 1, false },
        AnimationData{ AnimationType::ClipOut, "clipout.poa", 3, false },
        AnimationData{ AnimationType::ClipIn, "clipin.poa", 3, false },
        AnimationData{ AnimationType::SlideBack, "slideback.poa", 2, false },
        AnimationData{ AnimationType::Change, "change.poa", 1, false },
        AnimationData{ AnimationType::ThrowWeapon, "wyrzuca.poa", 1, false },
        AnimationData{ AnimationType::WeaponNone, "bezbroni.poa", 3, false },
        AnimationData{ AnimationType::Punch, "bije.poa", 1, false },
        AnimationData{ AnimationType::ReloadBow, "strzala.poa", 1, false },
        AnimationData{ AnimationType::Barret, "barret.poa", 9, false },
        AnimationData{ AnimationType::Roll, "skokdolobrot.poa", 1, false },
        AnimationData{ AnimationType::RollBack, "skokdolobrottyl.poa", 1, false },
        AnimationData{ AnimationType::CrouchRunBack, "kucaidzietyl.poa", 2, true },
        AnimationData{ AnimationType::Cigar, "cigar.poa", 3, false },
        AnimationData{ AnimationType::Match, "match.poa", 3, false },
        AnimationData{ AnimationType::Smoke, "smoke.poa", 4, false },
        AnimationData{ AnimationType::Wipe, "wipe.poa", 4, false },
        AnimationData{ AnimationType::Groin, "krocze.poa", 2, false },
        AnimationData{ AnimationType::Piss, "szcza.poa", 8, false },
        AnimationData{ AnimationType::Mercy, "samo.poa", 3, false },
        AnimationData{ AnimationType::Mercy2, "samo2.poa", 3, false },
        AnimationData{ AnimationType::TakeOff, "takeoff.poa", 2, false },
        AnimationData{ AnimationType::Prone, "lezy.poa", 1, false },
        AnimationData{ AnimationType::Victory, "cieszy.poa", 3, false },
        AnimationData{ AnimationType::Aim, "celuje.poa", 2, false },
        AnimationData{ AnimationType::HandsUpAim, "gora.poa", 2, false },
        AnimationData{ AnimationType::ProneMove, "lezyidzie.poa", 2, true },
        AnimationData{ AnimationType::GetUp, "wstaje.poa", 1, false },
        AnimationData{ AnimationType::AimRecoil, "celujeodrzut.poa", 1, false },
        AnimationData{ AnimationType::HandsUpRecoil, "goraodrzut.poa", 1, false },
        AnimationData{ AnimationType::Melee, "kolba.poa", 1, false },
        AnimationData{ AnimationType::Own, "rucha.poa", 3, false }
    };

    return animations.at(std::to_underlying(animation_type));
}

unsigned int Animations::GetFramesCount(AnimationType animation_type)
{
    return Get(animation_type).GetFrames().size();
}

AnimationData::AnimationData(AnimationType id, const std::string& file_name, int speed, bool looped)
    : id_(id)
    , looped_(looped)
    , speed_(speed)
{
    spdlog::info("AnimationData");

    std::filesystem::path file_path = "anims/" + file_name;
    std::ifstream ifs;
    ifs.open(file_path, std::ios::in);
    if (!ifs) {
        std::string message = "Could not open file: ";
        message += file_path.string();
        throw std::runtime_error(message.c_str());
    }
    spdlog::info("Animation: {}", file_path.string());

    std::string line;
    std::vector<glm::vec2> positions;

    GetlineSafe(ifs, line);
    while (!ifs.eof() && (line != "ENDFILE")) {
        if (line == "NEXTFRAME") {
            frames_.push_back({ positions });
            positions.clear();
        } else {
            unsigned int point = std::stoul(line);
            if (point != positions.size() + 1) {
                spdlog::error("Error in Animation loading: {} != {}", point, positions.size() + 1);
            }

            float x = NAN;
            float y = NAN;
            float z = NAN;
            GetlineSafe(ifs, line);
            x = std::stof(line);
            GetlineSafe(ifs, line);
            y = std::stof(line);
            GetlineSafe(ifs, line);
            z = std::stof(line);

            positions.emplace_back(-3.0F * x / 1.1F, -3.0 * z);
        }

        GetlineSafe(ifs, line);
    }

    frames_.push_back({ positions });
    positions.clear();
}

AnimationState::AnimationState(AnimationType id)
    : id_(id)
    , looped_(Animations::Get(id).GetLooped())
    , speed_(Animations::Get(id).GetSpeed())
    , count_(0)
    , frame_(1)
{
}

void AnimationState::DoAnimation()
{
    count_ += 1;

    if (count_ == speed_) {
        count_ = 0;
        frame_ += 1;

        if (frame_ > GetFramesCount()) {
            if (looped_) {
                frame_ = 1;
            } else {
                frame_ = GetFramesCount();
            }
        }
    }
}

const glm::vec2& AnimationState::GetPosition(unsigned int index) const
{
    return Animations::Get(id_).GetFrames()[frame_ - 1].positions[index - 1];
}

unsigned int AnimationState::GetFramesCount() const
{
    return Animations::GetFramesCount(id_);
}

bool AnimationState::IsAny(const std::vector<AnimationType>& animations) const
{
    return std::ranges::any_of(
      animations, [this](AnimationType animation_type) { return animation_type == id_; });
}
} // namespace Soldank
