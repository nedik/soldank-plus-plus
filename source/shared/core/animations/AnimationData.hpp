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

constexpr const std::string_view ANIMATION_BASE_DIRECTORY_PATH = "anims/";
constexpr const std::string_view ANIMATION_STAND_FILE = "stoi.poa";
constexpr const std::string_view ANIMATION_RUN_FILE = "biega.poa";
constexpr const std::string_view ANIMATION_RUN_BACK_FILE = "biegatyl.poa";
constexpr const std::string_view ANIMATION_JUMP_FILE = "skok.poa";
constexpr const std::string_view ANIMATION_JUMP_SIDE_FILE = "skokwbok.poa";
constexpr const std::string_view ANIMATION_FALL_FILE = "spada.poa";
constexpr const std::string_view ANIMATION_CROUCH_FILE = "kuca.poa";
constexpr const std::string_view ANIMATION_CROUCH_RUN_FILE = "kucaidzie.poa";
constexpr const std::string_view ANIMATION_RELOAD_FILE = "laduje.poa";
constexpr const std::string_view ANIMATION_THROW_FILE = "rzuca.poa";
constexpr const std::string_view ANIMATION_RECOIL_FILE = "odrzut.poa";
constexpr const std::string_view ANIMATION_SMALL_RECOIL_FILE = "odrzut2.poa";
constexpr const std::string_view ANIMATION_SHOTGUN_FILE = "shotgun.poa";
constexpr const std::string_view ANIMATION_CLIP_OUT_FILE = "clipout.poa";
constexpr const std::string_view ANIMATION_CLIP_IN_FILE = "clipin.poa";
constexpr const std::string_view ANIMATION_SLIDE_BACK_FILE = "slideback.poa";
constexpr const std::string_view ANIMATION_CHANGE_FILE = "change.poa";
constexpr const std::string_view ANIMATION_THROW_WEAPON_FILE = "wyrzuca.poa";
constexpr const std::string_view ANIMATION_WEAPON_NONE_FILE = "bezbroni.poa";
constexpr const std::string_view ANIMATION_PUNCH_FILE = "bije.poa";
constexpr const std::string_view ANIMATION_RELOAD_BOW_FILE = "strzala.poa";
constexpr const std::string_view ANIMATION_BARRET_FILE = "barret.poa";
constexpr const std::string_view ANIMATION_ROLL_FILE = "skokdolobrot.poa";
constexpr const std::string_view ANIMATION_ROLL_BACK_FILE = "skokdolobrottyl.poa";
constexpr const std::string_view ANIMATION_CROUCH_RUN_BACK_FILE = "kucaidzietyl.poa";
constexpr const std::string_view ANIMATION_CIGAR_FILE = "cigar.poa";
constexpr const std::string_view ANIMATION_MATCH_FILE = "match.poa";
constexpr const std::string_view ANIMATION_SMOKE_FILE = "smoke.poa";
constexpr const std::string_view ANIMATION_WIPE_FILE = "wipe.poa";
constexpr const std::string_view ANIMATION_GROIN_FILE = "krocze.poa";
constexpr const std::string_view ANIMATION_PISS_FILE = "szcza.poa";
constexpr const std::string_view ANIMATION_MERCY_FILE = "samo.poa";
constexpr const std::string_view ANIMATION_MERCY2_FILE = "samo2.poa";
constexpr const std::string_view ANIMATION_TAKE_OFF_FILE = "takeoff.poa";
constexpr const std::string_view ANIMATION_PRONE_FILE = "lezy.poa";
constexpr const std::string_view ANIMATION_VICTORY_FILE = "cieszy.poa";
constexpr const std::string_view ANIMATION_AIM_FILE = "celuje.poa";
constexpr const std::string_view ANIMATION_HANDS_UP_AIM_FILE = "gora.poa";
constexpr const std::string_view ANIMATION_PRONE_MOVE_FILE = "lezyidzie.poa";
constexpr const std::string_view ANIMATION_GET_UP_FILE = "wstaje.poa";
constexpr const std::string_view ANIMATION_AIM_RECOIL_FILE = "celujeodrzut.poa";
constexpr const std::string_view ANIMATION_HANDS_UP_RECOIL_FILE = "goraodrzut.poa";
constexpr const std::string_view ANIMATION_MELEE_FILE = "kolba.poa";
constexpr const std::string_view ANIMATION_OWN_FILE = "rucha.poa";

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

    std::shared_ptr<const AnimationData> Get(AnimationType animation_type) const;

private:
    std::vector<std::shared_ptr<const AnimationData>> animation_datas_;
};
} // namespace Soldank

#endif
