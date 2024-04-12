#ifndef __ANIMATION_HPP__
#define __ANIMATION_HPP__

#include "core/math/Glm.hpp"

#include <vector>
#include <string>

namespace Soldank
{
// TODO: move to separate files
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
    AnimationData(AnimationType id, const std::string& file_name, int speed, bool looped);

    bool GetLooped() const { return looped_; }
    int GetSpeed() const { return speed_; }
    const std::vector<AnimationFrame>& GetFrames() const { return frames_; }

private:
    AnimationType id_;
    bool looped_;
    int speed_;
    std::vector<AnimationFrame> frames_;
};

class Animations
{
public:
    static const AnimationData& Get(AnimationType animation_type);
    static unsigned int GetFramesCount(AnimationType animation_type);
};

class AnimationState
{
public:
    AnimationState(AnimationType id);

    void DoAnimation();
    const glm::vec2& GetPosition(unsigned int index) const;
    unsigned int GetFramesCount() const;
    bool IsAny(const std::vector<AnimationType>& animations) const;

    AnimationType GetType() const { return id_; };

    int GetSpeed() const { return speed_; }
    void SetSpeed(int new_speed) { speed_ = new_speed; }

    unsigned int GetFrame() const { return frame_; }
    void SetFrame(unsigned int new_frame) { frame_ = new_frame; }
    void SetNextFrame() { frame_++; }

    // TODO: these should be private

private:
    AnimationType id_;
    bool looped_;
    int speed_;
    int count_;
    unsigned int frame_;
};
} // namespace Soldank

#endif
