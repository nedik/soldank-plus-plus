#include "SoldierMovementSimulation.hpp"

namespace SoldankTesting
{
SoldierMovementSimulation::SoldierMovementSimulation(const Soldank::IFileReader& file_reader)
{
    Soldank::State& state = state_manager_.GetState();
    auto map =
      SoldankTesting::MapBuilder::Empty()
        ->AddPolygon(
          { 0.0F, 0.0F }, { 100.0F, 0.0F }, { 50.0F, 50.0F }, Soldank::PMSPolygonType::Normal)
        ->Build();
    state.map = *map;
    animation_data_manager_.LoadAllAnimationDatas(file_reader);
    std::vector<Soldank::Weapon> weapons{
        { Soldank::WeaponParametersFactory::GetParameters(
          Soldank::WeaponType::DesertEagles, false, file_reader) },
        { Soldank::WeaponParametersFactory::GetParameters(
          Soldank::WeaponType::Knife, false, file_reader) },
        { Soldank::WeaponParametersFactory::GetParameters(
          Soldank::WeaponType::FragGrenade, false, file_reader) }
    };
    state.soldiers.emplace_back(
      0,
      glm::vec2{ 0.0F, -29.0F },
      animation_data_manager_,
      Soldank::ParticleSystem::Load(Soldank::ParticleSystemType::Soldier, file_reader),
      weapons);
}

void SoldierMovementSimulation::HoldRight()
{
    Soldank::State& state = state_manager_.GetState();
    auto& current_soldier = *state.soldiers.begin();
    current_soldier.control.right = true;
}

void SoldierMovementSimulation::HoldLeft()
{
    Soldank::State& state = state_manager_.GetState();
    auto& current_soldier = *state.soldiers.begin();
    current_soldier.control.left = true;
}

void SoldierMovementSimulation::HoldJump()
{
    Soldank::State& state = state_manager_.GetState();
    auto& current_soldier = *state.soldiers.begin();
    current_soldier.control.up = true;
}

void SoldierMovementSimulation::LookLeft()
{
    state_manager_.ChangeSoldierMousePosition(0, { 0.0F, 0.0F });
}

void SoldierMovementSimulation::LookRight()
{
    state_manager_.ChangeSoldierMousePosition(0, { 640.0F, 0.0F });
}

void SoldierMovementSimulation::AddSoldierExpectedAnimationState(
  unsigned int tick,
  const SoldierExpectedAnimationState& soldier_expected_animation_state)
{
    if (!animations_to_check_at_tick_.contains(tick)) {
        animations_to_check_at_tick_[tick] = {};
    }

    animations_to_check_at_tick_.at(tick).push_back(soldier_expected_animation_state);
}

void SoldierMovementSimulation::RunUntilSoldierOnGround()
{
    Soldank::State& state = state_manager_.GetState();
    auto& current_soldier = *state.soldiers.begin();
    while (!current_soldier.on_ground) {
        std::vector<Soldank::BulletParams> bullet_emitter;
        Soldank::SoldierPhysics::Update(
          state, current_soldier, animation_data_manager_, bullet_emitter);
    }
}

void SoldierMovementSimulation::RunFor(unsigned int ticks_to_run)
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

void SoldierMovementSimulation::CheckSoldierAnimationStates(
  const Soldank::Soldier& soldier,
  const SoldierExpectedAnimationStates& expected_animation_states)
{
    for (const auto& expected_animation_state : expected_animation_states) {
        CheckSoldierAnimationState(soldier, expected_animation_state);
    }
}

void SoldierMovementSimulation::CheckSoldierAnimationState(
  const Soldank::Soldier& soldier,
  const SoldierExpectedAnimationState& expected_animation_state)
{
    switch (expected_animation_state.part) {
        case SoldierAnimationPart::Legs: {
            EXPECT_EQ(soldier.legs_animation.GetType(),
                      expected_animation_state.expected_animation_type);
            EXPECT_EQ(soldier.legs_animation.GetFrame(), expected_animation_state.expected_frame);
            EXPECT_EQ(soldier.legs_animation.GetSpeed(), expected_animation_state.expected_speed);
            break;
        }
        case SoldierAnimationPart::Body: {
            EXPECT_EQ(soldier.body_animation.GetType(),
                      expected_animation_state.expected_animation_type);
            EXPECT_EQ(soldier.body_animation.GetFrame(), expected_animation_state.expected_frame);
            EXPECT_EQ(soldier.body_animation.GetSpeed(), expected_animation_state.expected_speed);
            break;
        }
    }
}
} // namespace SoldankTesting
