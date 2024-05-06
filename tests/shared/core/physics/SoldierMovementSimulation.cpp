#include "SoldierMovementSimulation.hpp"
#include "core/state/Control.hpp"

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
    state_manager_.ChangeSoldierControlActionState(0, Soldank::ControlActionType::MoveRight, true);
}

void SoldierMovementSimulation::HoldLeft()
{
    state_manager_.ChangeSoldierControlActionState(0, Soldank::ControlActionType::MoveLeft, true);
}

void SoldierMovementSimulation::HoldJump()
{
    state_manager_.ChangeSoldierControlActionState(0, Soldank::ControlActionType::Jump, true);
}

void SoldierMovementSimulation::HoldRightAt(unsigned int tick)
{
    AddControlToChangeAt(
      tick, { .control_type = Soldank::ControlActionType::MoveRight, .new_state = true });
}

void SoldierMovementSimulation::HoldLeftAt(unsigned int tick)
{
    AddControlToChangeAt(
      tick, { .control_type = Soldank::ControlActionType::MoveLeft, .new_state = true });
}

void SoldierMovementSimulation::HoldJumpAt(unsigned int tick)
{
    AddControlToChangeAt(tick,
                         { .control_type = Soldank::ControlActionType::Jump, .new_state = true });
}

void SoldierMovementSimulation::LookLeft()
{
    is_soldier_looking_left_ = true;
    TurnSoldierLeft();
}

void SoldierMovementSimulation::LookRight()
{
    is_soldier_looking_left_ = false;
    TurnSoldierRight();
}

void SoldierMovementSimulation::HoldJetsAt(unsigned int tick)
{
    AddControlToChangeAt(
      tick, { .control_type = Soldank::ControlActionType::UseJets, .new_state = true });
}

void SoldierMovementSimulation::ReleaseRightAt(unsigned int tick)
{
    AddControlToChangeAt(
      tick, { .control_type = Soldank::ControlActionType::MoveRight, .new_state = false });
}

void SoldierMovementSimulation::ReleaseLeftAt(unsigned int tick)
{
    AddControlToChangeAt(
      tick, { .control_type = Soldank::ControlActionType::MoveLeft, .new_state = false });
}

void SoldierMovementSimulation::ReleaseJumpAt(unsigned int tick)
{
    AddControlToChangeAt(tick,
                         { .control_type = Soldank::ControlActionType::Jump, .new_state = false });
}

void SoldierMovementSimulation::ReleaseJetsAt(unsigned int tick)
{
    AddControlToChangeAt(
      tick, { .control_type = Soldank::ControlActionType::UseJets, .new_state = false });
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
        if (controls_to_change_at_tick_.contains(current_tick)) {
            const auto& controls_to_change = controls_to_change_at_tick_.at(current_tick);
            for (const auto& control_to_change : controls_to_change) {
                state_manager_.ChangeSoldierControlActionState(
                  0, control_to_change.control_type, control_to_change.new_state);
            }
        }
        if (is_soldier_looking_left_) {
            TurnSoldierLeft();
        } else {
            TurnSoldierRight();
        }

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

void SoldierMovementSimulation::AddControlToChangeAt(unsigned int tick,
                                                     const ControlToChange& control_to_change)
{
    if (!controls_to_change_at_tick_.contains(tick)) {
        controls_to_change_at_tick_[tick] = {};
    }

    controls_to_change_at_tick_.at(tick).push_back(control_to_change);
}

void SoldierMovementSimulation::TurnSoldierLeft()
{
    state_manager_.ChangeSoldierMousePosition(0, { 0.0F, 0.0F });
}

void SoldierMovementSimulation::TurnSoldierRight()
{
    state_manager_.ChangeSoldierMousePosition(0, { 640.0F, 0.0F });
}
} // namespace SoldankTesting
