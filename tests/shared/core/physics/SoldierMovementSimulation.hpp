#include "core/physics/SoldierPhysics.hpp"
#include "core/animations/AnimationState.hpp"
#include "core/state/StateManager.hpp"
#include "core/entities/WeaponParametersFactory.hpp"

#include "shared_lib_testing/MapBuilder.hpp"

#include <gtest/gtest.h>

namespace SoldankTesting
{
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

class SoldierMovementSimulation
{
public:
    SoldierMovementSimulation(const Soldank::IFileReader& file_reader);

    void HoldRight();

    void HoldLeft();

    void HoldJump();

    void LookLeft();
    void LookRight();

    void AddSoldierExpectedAnimationState(
      unsigned int tick,
      const SoldierExpectedAnimationState& soldier_expected_animation_state);

    void RunUntilSoldierOnGround();

    void RunFor(unsigned int ticks_to_run);

private:
    using SoldierExpectedAnimationStates = std::vector<SoldierExpectedAnimationState>;

    static void CheckSoldierAnimationStates(
      const Soldank::Soldier& soldier,
      const SoldierExpectedAnimationStates& expected_animation_states);

    static void CheckSoldierAnimationState(
      const Soldank::Soldier& soldier,
      const SoldierExpectedAnimationState& expected_animation_state);

    Soldank::StateManager state_manager_;
    Soldank::AnimationDataManager animation_data_manager_;

    std::unordered_map<unsigned int, SoldierExpectedAnimationStates> animations_to_check_at_tick_;
};
} // namespace SoldankTesting
