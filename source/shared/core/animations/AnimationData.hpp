#ifndef __ANIMATION_DATA_HPP__
#define __ANIMATION_DATA_HPP__

#include "core/data/FileReader.hpp"
#include "core/data/IFileReader.hpp"

#include "core/math/Glm.hpp"

#include <vector>
#include <string>
#include <memory>

namespace Soldank
{
enum class AnimationType
{
    Stand = 0,
    Run,
    RunBack,
    Jump,
    JumpSide,
    Fall,
    Crouch,
    CrouchRun,
    Reload,
    Throw,
    Recoil,
    SmallRecoil,
    Shotgun,
    ClipOut,
    ClipIn,
    SlideBack,
    Change,
    ThrowWeapon,
    WeaponNone,
    Punch,
    ReloadBow,
    Barret,
    Roll,
    RollBack,
    CrouchRunBack,
    Cigar,
    Match,
    Smoke,
    Wipe,
    Groin,
    Piss,
    Mercy,
    Mercy2,
    TakeOff,
    Prone,
    Victory,
    Aim,
    HandsUpAim,
    ProneMove,
    GetUp,
    AimRecoil,
    HandsUpRecoil,
    Melee,
    Own,
};

struct AnimationFrame
{
    std::vector<glm::vec2> positions;
};

class AnimationData
{
public:
    AnimationData(AnimationType animation_type,
                  bool looped,
                  int speed,
                  const std::vector<AnimationFrame>& frames)
        : animation_type_(animation_type)
        , looped_(looped)
        , speed_(speed)
        , frames_(frames){};

    AnimationType GetAnimationType() const { return animation_type_; }
    bool GetLooped() const { return looped_; }
    int GetSpeed() const { return speed_; }
    const std::vector<AnimationFrame>& GetFrames() const { return frames_; }

private:
    AnimationType animation_type_;
    bool looped_;
    int speed_;
    std::vector<AnimationFrame> frames_;
};

class AnimationDataManager
{
public:
    // TODO: add error handling for when animation could not be read from file
    // TODO: add error handling for when animation of type animation_type had already been loaded
    // before
    void LoadAnimationData(AnimationType animation_type,
                           const std::string& file_path,
                           bool looped,
                           int speed,
                           const IFileReader& file_reader = FileReader());
    void LoadAllAnimationDatas(const IFileReader& file_reader = FileReader());

    std::shared_ptr<const AnimationData> Get(AnimationType animation_type);
    unsigned int GetFramesCount(AnimationType animation_type);

private:
    std::vector<std::shared_ptr<const AnimationData>> animation_datas_;
};
} // namespace Soldank

#endif
