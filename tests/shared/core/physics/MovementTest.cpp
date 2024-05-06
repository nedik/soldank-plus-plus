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

#include "SoldierMovementSimulation.hpp"

#include <gtest/gtest.h>

#include <vector>
#include <algorithm>
#include <unordered_map>
#include <filesystem>
#include <fstream>

std::filesystem::path test_file_path;

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

TEST(MovementTest, TestRunBackRight)
{
    SoldankTesting::SoldierMovementSimulation simulation{ FileReaderForTestsWorkingDirectory() };
    simulation.RunUntilSoldierOnGround();
    simulation.LookLeft();
    simulation.RunFor(10);
    simulation.HoldRight();
    simulation.AddSoldierExpectedAnimationState(
      1,
      { .part = SoldankTesting::SoldierAnimationPart::Legs,
        .expected_animation_type = Soldank::AnimationType::RunBack,
        .expected_frame = 3,
        .expected_speed = 1 });
    simulation.RunFor(10);
}

TEST(MovementTest, TestRunBackLeft)
{
    SoldankTesting::SoldierMovementSimulation simulation{ FileReaderForTestsWorkingDirectory() };
    simulation.RunUntilSoldierOnGround();
    simulation.LookRight();
    simulation.RunFor(10);
    simulation.HoldLeft();
    simulation.AddSoldierExpectedAnimationState(
      1,
      { .part = SoldankTesting::SoldierAnimationPart::Legs,
        .expected_animation_type = Soldank::AnimationType::RunBack,
        .expected_frame = 3,
        .expected_speed = 1 });
    simulation.RunFor(10);
}

TEST(MovementTest, TestFallAndJumpSideLeft)
{
    SoldankTesting::SoldierMovementSimulation simulation{ FileReaderForTestsWorkingDirectory() };
    simulation.RunUntilSoldierOnGround();
    simulation.HoldLeft();
    simulation.HoldJump();
    simulation.AddSoldierExpectedAnimationState(
      0,
      { .part = SoldankTesting::SoldierAnimationPart::Legs,
        .expected_animation_type = Soldank::AnimationType::Run,
        .expected_frame = 2,
        .expected_speed = 1 });
    simulation.AddSoldierExpectedAnimationState(
      3,
      { .part = SoldankTesting::SoldierAnimationPart::Legs,
        .expected_animation_type = Soldank::AnimationType::JumpSide,
        .expected_frame = 2,
        .expected_speed = 1 });
    simulation.RunFor(10);
}

TEST(MovementTest, TestJumpSideLeft)
{
    SoldankTesting::SoldierMovementSimulation simulation{ FileReaderForTestsWorkingDirectory() };
    simulation.RunUntilSoldierOnGround();
    simulation.RunFor(10);
    simulation.HoldLeft();
    simulation.HoldJump();
    simulation.AddSoldierExpectedAnimationState(
      0,
      { .part = SoldankTesting::SoldierAnimationPart::Legs,
        .expected_animation_type = Soldank::AnimationType::JumpSide,
        .expected_frame = 2,
        .expected_speed = 1 });
    simulation.AddSoldierExpectedAnimationState(
      4,
      { .part = SoldankTesting::SoldierAnimationPart::Legs,
        .expected_animation_type = Soldank::AnimationType::JumpSide,
        .expected_frame = 6,
        .expected_speed = 1 });
    simulation.RunFor(10);
}

TEST(MovementTest, TestLateBackflipLeftLookingRight)
{
    SoldankTesting::SoldierMovementSimulation simulation{ FileReaderForTestsWorkingDirectory() };
    simulation.RunUntilSoldierOnGround();
    simulation.LookRight();
    simulation.RunFor(10);
    simulation.HoldLeft();
    simulation.HoldJumpAt(1);
    simulation.HoldJetsAt(32);
    simulation.ReleaseLeftAt(35);
    simulation.ReleaseJetsAt(35);
    simulation.ReleaseJumpAt(35);
    simulation.AddSoldierExpectedAnimationState(
      0,
      { .part = SoldankTesting::SoldierAnimationPart::Legs,
        .expected_animation_type = Soldank::AnimationType::RunBack,
        .expected_frame = 2,
        .expected_speed = 1 });
    simulation.AddSoldierExpectedAnimationState(
      1,
      { .part = SoldankTesting::SoldierAnimationPart::Legs,
        .expected_animation_type = Soldank::AnimationType::JumpSide,
        .expected_frame = 2,
        .expected_speed = 1 });
    simulation.AddSoldierExpectedAnimationState(
      32,
      { .part = SoldankTesting::SoldierAnimationPart::Legs,
        .expected_animation_type = Soldank::AnimationType::RollBack,
        .expected_frame = 2,
        .expected_speed = 1 });
    simulation.AddSoldierExpectedAnimationState(
      32,
      { .part = SoldankTesting::SoldierAnimationPart::Body,
        .expected_animation_type = Soldank::AnimationType::RollBack,
        .expected_frame = 2,
        .expected_speed = 1 });
    simulation.AddSoldierExpectedAnimationState(
      63,
      { .part = SoldankTesting::SoldierAnimationPart::Body,
        .expected_animation_type = Soldank::AnimationType::Stand,
        .expected_frame = 1,
        .expected_speed = 1 });
    simulation.AddSoldierExpectedAnimationState(
      64,
      { .part = SoldankTesting::SoldierAnimationPart::Legs,
        .expected_animation_type = Soldank::AnimationType::Fall,
        .expected_frame = 2,
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
