#include "core/animations/AnimationData.hpp"
#include "shared_lib_testing/MapBuilder.hpp"
#include "core/map/PMSEnums.hpp"
#include "core/map/PMSStructs.hpp"

#include "core/physics/SoldierPhysics.hpp"

#include "core/animations/AnimationState.hpp"
#include "core/entities/Soldier.hpp"
#include "core/entities/WeaponParametersFactory.hpp"
#include "core/state/StateManager.hpp"
#include "core/data/IFileReader.hpp"

#include <gtest/gtest.h>

#include <vector>
#include <algorithm>
#include <unordered_map>
#include <filesystem>
#include <fstream>

std::filesystem::path test_file_path;

enum class SoldierAnimationPart
{
    Legs = 0,
    Body
};

struct SoldierExpectedAnimationState
{
    SoldierAnimationPart part;
    Soldank::AnimationType expected_animation_type;
    unsigned int expected_frame;
    int expected_speed;
};

class FileReaderForTestsWorkingDirectory : public Soldank::IFileReader
{
public:
    std::expected<std::string, Soldank::FileReaderError> Read(
      const std::string& file_path,
      std::ios_base::openmode mode = std::ios_base::in) const override
    {
        std::filesystem::path absolute_file_path = test_file_path;
        absolute_file_path.append(file_path);
        std::ifstream file_to_read(absolute_file_path, mode);
        if (!file_to_read.is_open()) {
            return std::unexpected(Soldank::FileReaderError::FileNotFound);
        }

        std::stringstream buffer;
        buffer << file_to_read.rdbuf();

        if (buffer.bad()) {
            return std::unexpected(Soldank::FileReaderError::BufferError);
        }

        return buffer.str();
    }
};

class Simulation
{
public:
    Simulation()
    {
        Soldank::State& state = state_manager_.GetState();
        auto map =
          SoldankTesting::MapBuilder::Empty()
            ->AddPolygon(
              { 0.0F, 0.0F }, { 100.0F, 0.0F }, { 50.0F, 50.0F }, Soldank::PMSPolygonType::Normal)
            ->Build();
        state.map = *map;
        animation_data_manager_.LoadAllAnimationDatas(FileReaderForTestsWorkingDirectory());
        std::vector<Soldank::Weapon> weapons{
            { Soldank::WeaponParametersFactory::GetParameters(
              Soldank::WeaponType::DesertEagles, false, FileReaderForTestsWorkingDirectory()) },
            { Soldank::WeaponParametersFactory::GetParameters(
              Soldank::WeaponType::Knife, false, FileReaderForTestsWorkingDirectory()) },
            { Soldank::WeaponParametersFactory::GetParameters(
              Soldank::WeaponType::FragGrenade, false, FileReaderForTestsWorkingDirectory()) }
        };
        state.soldiers.emplace_back(
          0,
          glm::vec2{ 0.0F, -29.0F },
          animation_data_manager_,
          Soldank::ParticleSystem::Load(Soldank::ParticleSystemType::Soldier,
                                        FileReaderForTestsWorkingDirectory()),
          weapons);
    }

    void RunRight()
    {
        Soldank::State& state = state_manager_.GetState();
        auto& current_soldier = *state.soldiers.begin();
        current_soldier.control.right = true;
    }

    void AddSoldierExpectedAnimationState(
      unsigned int tick,
      const SoldierExpectedAnimationState& soldier_expected_animation_state)
    {
        if (!animations_to_check_at_tick_.contains(tick)) {
            animations_to_check_at_tick_[tick] = {};
        }

        animations_to_check_at_tick_.at(tick).push_back(soldier_expected_animation_state);
    }

    void RunUntilSoldierOnGround()
    {
        Soldank::State& state = state_manager_.GetState();
        auto& current_soldier = *state.soldiers.begin();
        while (!current_soldier.on_ground) {
            std::vector<Soldank::BulletParams> bullet_emitter;
            Soldank::SoldierPhysics::Update(
              state, current_soldier, animation_data_manager_, bullet_emitter);
        }
    }

    void RunFor(unsigned int ticks_to_run)
    {
        Soldank::State& state = state_manager_.GetState();
        auto& current_soldier = *state.soldiers.begin();
        for (unsigned int current_tick = 0; current_tick < ticks_to_run; current_tick++) {
            std::vector<Soldank::BulletParams> bullet_emitter;
            Soldank::SoldierPhysics::Update(
              state, current_soldier, animation_data_manager_, bullet_emitter);
            if (animations_to_check_at_tick_.contains(current_tick)) {
                CheckSoldierAnimationStates(current_soldier,
                                            animations_to_check_at_tick_.at(current_tick));
            }
        }
    }

private:
    using SoldierExpectedAnimationStates = std::vector<SoldierExpectedAnimationState>;

    static void CheckSoldierAnimationStates(
      const Soldank::Soldier& soldier,
      const SoldierExpectedAnimationStates& expected_animation_states)
    {
        for (const auto& expected_animation_state : expected_animation_states) {
            CheckSoldierAnimationState(soldier, expected_animation_state);
        }
    }

    static void CheckSoldierAnimationState(
      const Soldank::Soldier& soldier,
      const SoldierExpectedAnimationState& expected_animation_state)
    {
        switch (expected_animation_state.part) {
            case SoldierAnimationPart::Legs: {
                EXPECT_EQ(soldier.legs_animation.GetType(),
                          expected_animation_state.expected_animation_type);
                EXPECT_EQ(soldier.legs_animation.GetFrame(),
                          expected_animation_state.expected_frame);
                EXPECT_EQ(soldier.legs_animation.GetSpeed(),
                          expected_animation_state.expected_speed);
                break;
            }
            case SoldierAnimationPart::Body: {
                EXPECT_EQ(soldier.body_animation.GetType(),
                          expected_animation_state.expected_animation_type);
                EXPECT_EQ(soldier.body_animation.GetFrame(),
                          expected_animation_state.expected_frame);
                EXPECT_EQ(soldier.body_animation.GetSpeed(),
                          expected_animation_state.expected_speed);
                break;
            }
        }
    }

    Soldank::StateManager state_manager_;
    Soldank::AnimationDataManager animation_data_manager_;

    std::unordered_map<unsigned int, SoldierExpectedAnimationStates> animations_to_check_at_tick_;
};

TEST(MovementTest, TestRunBackRight)
{
    Simulation simulation;
    simulation.RunUntilSoldierOnGround();
    simulation.RunRight();
    simulation.AddSoldierExpectedAnimationState(
      1,
      { .part = SoldierAnimationPart::Legs,
        .expected_animation_type = Soldank::AnimationType::RunBack,
        .expected_frame = 3,
        .expected_speed = 1 });
    simulation.RunFor(10);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    assert(argc == 1);
    test_file_path = argv[0];
    test_file_path = test_file_path.remove_filename();
    return RUN_ALL_TESTS();
}
