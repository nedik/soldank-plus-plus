#include "core/animations/AnimationData.hpp"

#include "core/utility/Getline.hpp"

#include "spdlog/spdlog.h"

#include <algorithm>
#include <ranges>
#include <string_view>
#include <array>

namespace Soldank
{
void AnimationDataManager::LoadAnimationData(AnimationType animation_type,
                                             const std::string& file_path,
                                             bool looped,
                                             int speed,
                                             const IFileReader& file_reader)
{
    spdlog::info("LoadAnimationData");

    auto file_data = file_reader.Read(file_path);
    if (!file_data.has_value()) {
        spdlog::critical("Animation file not found {}", file_path);
        std::string message = "Could not open file: " + file_path;
        throw std::runtime_error(message.c_str());
    }
    std::stringstream data_buffer{ *file_data };
    spdlog::info("Animation: {}", file_path);

    std::string line;
    std::vector<glm::vec2> positions;
    std::vector<AnimationFrame> frames;

    GetlineSafe(data_buffer, line);
    while (!data_buffer.eof() && (line != "ENDFILE")) {
        if (line == "NEXTFRAME") {
            frames.push_back({ positions });
            positions.clear();
        } else {
            unsigned int point = std::stoul(line);
            if (point != positions.size() + 1) {
                spdlog::error("Error in Animation loading: {} != {}", point, positions.size() + 1);
            }

            GetlineSafe(data_buffer, line);
            float x = std::stof(line);
            GetlineSafe(data_buffer, line); // y is not used for some reason
            GetlineSafe(data_buffer, line);
            float z = std::stof(line);

            positions.emplace_back(-3.0F * x / 1.1F, -3.0 * z);
        }

        GetlineSafe(data_buffer, line);
    }

    frames.push_back({ positions });

    animation_datas_.push_back(
      std::make_shared<AnimationData>(animation_type, looped, speed, frames));
}

void AnimationDataManager::LoadAllAnimationDatas(const IFileReader& file_reader)
{
    const auto get_file_path = [](std::string_view file_name) {
        return std::string{ ANIMATION_BASE_DIRECTORY_PATH } + std::string{ file_name };
    };
    // clang-format off
    LoadAnimationData(AnimationType::Stand,          get_file_path(ANIMATION_STAND_FILE),           true,  3, file_reader);
    LoadAnimationData(AnimationType::Run,            get_file_path(ANIMATION_RUN_FILE),             true,  1, file_reader);
    LoadAnimationData(AnimationType::RunBack,        get_file_path(ANIMATION_RUN_BACK_FILE),        true,  1, file_reader);
    LoadAnimationData(AnimationType::Jump,           get_file_path(ANIMATION_JUMP_FILE),            false, 1, file_reader);
    LoadAnimationData(AnimationType::JumpSide,       get_file_path(ANIMATION_JUMP_SIDE_FILE),       false, 1, file_reader);
    LoadAnimationData(AnimationType::Fall,           get_file_path(ANIMATION_FALL_FILE),            false, 1, file_reader);
    LoadAnimationData(AnimationType::Crouch,         get_file_path(ANIMATION_CROUCH_FILE),          false, 1, file_reader);
    LoadAnimationData(AnimationType::CrouchRun,      get_file_path(ANIMATION_CROUCH_RUN_FILE),      true,  2, file_reader);
    LoadAnimationData(AnimationType::Reload,         get_file_path(ANIMATION_RELOAD_FILE),          false, 2, file_reader);
    LoadAnimationData(AnimationType::Throw,          get_file_path(ANIMATION_THROW_FILE),           false, 1, file_reader);
    LoadAnimationData(AnimationType::Recoil,         get_file_path(ANIMATION_RECOIL_FILE),          false, 1, file_reader);
    LoadAnimationData(AnimationType::SmallRecoil,    get_file_path(ANIMATION_SMALL_RECOIL_FILE),    false, 1, file_reader);
    LoadAnimationData(AnimationType::Shotgun,        get_file_path(ANIMATION_SHOTGUN_FILE),         false, 1, file_reader);
    LoadAnimationData(AnimationType::ClipOut,        get_file_path(ANIMATION_CLIP_OUT_FILE),        false, 3, file_reader);
    LoadAnimationData(AnimationType::ClipIn,         get_file_path(ANIMATION_CLIP_IN_FILE),         false, 3, file_reader);
    LoadAnimationData(AnimationType::SlideBack,      get_file_path(ANIMATION_SLIDE_BACK_FILE),      false, 2, file_reader);
    LoadAnimationData(AnimationType::Change,         get_file_path(ANIMATION_CHANGE_FILE),          false, 1, file_reader);
    LoadAnimationData(AnimationType::ThrowWeapon,    get_file_path(ANIMATION_THROW_WEAPON_FILE),    false, 1, file_reader);
    LoadAnimationData(AnimationType::WeaponNone,     get_file_path(ANIMATION_WEAPON_NONE_FILE),     false, 3, file_reader);
    LoadAnimationData(AnimationType::Punch,          get_file_path(ANIMATION_PUNCH_FILE),           false, 1, file_reader);
    LoadAnimationData(AnimationType::ReloadBow,      get_file_path(ANIMATION_RELOAD_BOW_FILE),      false, 1, file_reader);
    LoadAnimationData(AnimationType::Barret,         get_file_path(ANIMATION_BARRET_FILE),          false, 9, file_reader);
    LoadAnimationData(AnimationType::Roll,           get_file_path(ANIMATION_ROLL_FILE),            false, 1, file_reader);
    LoadAnimationData(AnimationType::RollBack,       get_file_path(ANIMATION_ROLL_BACK_FILE),       false, 1, file_reader);
    LoadAnimationData(AnimationType::CrouchRunBack,  get_file_path(ANIMATION_CROUCH_RUN_BACK_FILE), true,  2, file_reader);
    LoadAnimationData(AnimationType::Cigar,          get_file_path(ANIMATION_CIGAR_FILE),           false, 3, file_reader);
    LoadAnimationData(AnimationType::Match,          get_file_path(ANIMATION_MATCH_FILE),           false, 3, file_reader);
    LoadAnimationData(AnimationType::Smoke,          get_file_path(ANIMATION_SMOKE_FILE),           false, 4, file_reader);
    LoadAnimationData(AnimationType::Wipe,           get_file_path(ANIMATION_WIPE_FILE),            false, 4, file_reader);
    LoadAnimationData(AnimationType::Groin,          get_file_path(ANIMATION_GROIN_FILE),           false, 2, file_reader);
    LoadAnimationData(AnimationType::Piss,           get_file_path(ANIMATION_PISS_FILE),            false, 8, file_reader);
    LoadAnimationData(AnimationType::Mercy,          get_file_path(ANIMATION_MERCY_FILE),           false, 3, file_reader);
    LoadAnimationData(AnimationType::Mercy2,         get_file_path(ANIMATION_MERCY2_FILE),          false, 3, file_reader);
    LoadAnimationData(AnimationType::TakeOff,        get_file_path(ANIMATION_TAKE_OFF_FILE),        false, 2, file_reader);
    LoadAnimationData(AnimationType::Prone,          get_file_path(ANIMATION_PRONE_FILE),           false, 1, file_reader);
    LoadAnimationData(AnimationType::Victory,        get_file_path(ANIMATION_VICTORY_FILE),         false, 3, file_reader);
    LoadAnimationData(AnimationType::Aim,            get_file_path(ANIMATION_AIM_FILE),             false, 2, file_reader);
    LoadAnimationData(AnimationType::HandsUpAim,     get_file_path(ANIMATION_HANDS_UP_AIM_FILE),    false, 2, file_reader);
    LoadAnimationData(AnimationType::ProneMove,      get_file_path(ANIMATION_PRONE_MOVE_FILE),      true,  2, file_reader);
    LoadAnimationData(AnimationType::GetUp,          get_file_path(ANIMATION_GET_UP_FILE),          false, 1, file_reader);
    LoadAnimationData(AnimationType::AimRecoil,      get_file_path(ANIMATION_AIM_RECOIL_FILE),      false, 1, file_reader);
    LoadAnimationData(AnimationType::HandsUpRecoil,  get_file_path(ANIMATION_HANDS_UP_RECOIL_FILE), false, 1, file_reader);
    LoadAnimationData(AnimationType::Melee,          get_file_path(ANIMATION_MELEE_FILE),           false, 1, file_reader);
    LoadAnimationData(AnimationType::Own,            get_file_path(ANIMATION_OWN_FILE),             false, 3, file_reader);
    // clang-format on
}

std::shared_ptr<const AnimationData> AnimationDataManager::Get(AnimationType animation_type) const
{
    for (const auto& animation_data : animation_datas_) {
        if (animation_data->GetAnimationType() == animation_type) {
            return animation_data;
        }
    }

    // TODO: error handling
    std::unreachable();
}
} // namespace Soldank
