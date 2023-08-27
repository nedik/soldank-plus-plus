#include "Animation.hpp"

#include "core/utility/Getline.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <exception>
#include <cmath>

namespace Soldat
{
const AnimationData& Animations::Get(AnimationType animation_type)
{
    static std::unordered_map<AnimationType, AnimationData> animations{
        { Stand, { Stand, "stoi.poa", 3, true } },
        { Run, { Run, "biega.poa", 1, true } },
        { RunBack, { RunBack, "biegatyl.poa", 1, true } },
        { Jump, { Jump, "skok.poa", 1, false } },
        { JumpSide, { JumpSide, "skokwbok.poa", 1, false } },
        { Fall, { Fall, "spada.poa", 1, false } },
        { Crouch, { Crouch, "kuca.poa", 1, false } },
        { CrouchRun, { CrouchRun, "kucaidzie.poa", 2, true } },
        { Reload, { Reload, "laduje.poa", 2, false } },
        { Throw, { Throw, "rzuca.poa", 1, false } },
        { Recoil, { Recoil, "odrzut.poa", 1, false } },
        { SmallRecoil, { SmallRecoil, "odrzut2.poa", 1, false } },
        { Shotgun, { Shotgun, "shotgun.poa", 1, false } },
        { ClipOut, { ClipOut, "clipout.poa", 3, false } },
        { ClipIn, { ClipIn, "clipin.poa", 3, false } },
        { SlideBack, { SlideBack, "slideback.poa", 2, false } },
        { Change, { Change, "change.poa", 1, false } },
        { ThrowWeapon, { ThrowWeapon, "wyrzuca.poa", 1, false } },
        { WeaponNone, { WeaponNone, "bezbroni.poa", 3, false } },
        { Punch, { Punch, "bije.poa", 1, false } },
        { ReloadBow, { ReloadBow, "strzala.poa", 1, false } },
        { Barret, { Barret, "barret.poa", 9, false } },
        { Roll, { Roll, "skokdolobrot.poa", 1, false } },
        { RollBack, { RollBack, "skokdolobrottyl.poa", 1, false } },
        { CrouchRunBack, { CrouchRunBack, "kucaidzietyl.poa", 2, true } },
        { Cigar, { Cigar, "cigar.poa", 3, false } },
        { Match, { Match, "match.poa", 3, false } },
        { Smoke, { Smoke, "smoke.poa", 4, false } },
        { Wipe, { Wipe, "wipe.poa", 4, false } },
        { Groin, { Groin, "krocze.poa", 2, false } },
        { Piss, { Piss, "szcza.poa", 8, false } },
        { Mercy, { Mercy, "samo.poa", 3, false } },
        { Mercy2, { Mercy2, "samo2.poa", 3, false } },
        { TakeOff, { TakeOff, "takeoff.poa", 2, false } },
        { Prone, { Prone, "lezy.poa", 1, false } },
        { Victory, { Victory, "cieszy.poa", 3, false } },
        { Aim, { Aim, "celuje.poa", 2, false } },
        { HandsUpAim, { HandsUpAim, "gora.poa", 2, false } },
        { ProneMove, { ProneMove, "lezyidzie.poa", 2, true } },
        { GetUp, { GetUp, "wstaje.poa", 1, false } },
        { AimRecoil, { AimRecoil, "celujeodrzut.poa", 1, false } },
        { HandsUpRecoil, { HandsUpRecoil, "goraodrzut.poa", 1, false } },
        { Melee, { Melee, "kolba.poa", 1, false } },
        { Own, { Own, "rucha.poa", 3, false } },
    };

    return animations.at(animation_type);
}

unsigned int Animations::GetFramesCount(AnimationType animation_type)
{
    return Get(animation_type).GetFrames().size();
}

AnimationData::AnimationData(AnimationType id, const std::string& file_name, int speed, bool looped)
    : id_(id)
    , speed_(speed)
    , looped_(looped)
{
    std::cout << "AnimationData" << std::endl;
    std::filesystem::path file_path = "anims/" + file_name;
    std::ifstream ifs;
    ifs.open(file_path, std::ios::in);
    if (!ifs) {
        std::string message = "Could not open file: ";
        message += file_path.string();
        throw std::runtime_error(message.c_str());
    }
    std::cout << "Animation: " << file_path << std::endl;

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
                std::cerr << "Error in Animation loading: " << point
                          << " != " << positions.size() + 1 << std::endl;
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
} // namespace Soldat
