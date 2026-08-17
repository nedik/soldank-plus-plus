#include "core/math/Glm.hpp"

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <expected>
#include <ios>
#include <memory>
#include <string>
#include <vector>

import Shared.Core.Animations;
import Shared.Core.Data.IFileReader;
import Shared.Core.Entities.Bullet;
import Shared.Core.Entities.Soldier;
import Shared.Core.Map.Map;
import Shared.Core.Map.PMSEnums;
import Shared.Core.Physics.BulletPhysics;
import Shared.Core.Physics.Bullets.BulletCollision;
import Shared.Core.Physics.Bullets.BulletDamage;
import Shared.Core.Physics.Bullets.BulletTypes;
import Shared.Core.Physics.Particles;
import Shared.Core.Physics.PhysicsEvents;
import Shared.Core.State.StateManager;
import Shared.Core.Types.BulletType;
import Shared.Core.Types.TeamType;
import Shared.Core.Types.WeaponType;
import Testing.Framework.Shared.MapBuilder;

namespace
{
class AnimationDataReader final : public Soldank::IFileReader
{
public:
    std::expected<std::string, Soldank::FileReaderError> Read(
      const std::string& file_path,
      std::ios_base::openmode /*mode*/) const override
    {
        if (file_path != "stand.poa") {
            return std::unexpected(Soldank::FileReaderError::FileNotFound);
        }

        return "1\n0\n0\n0\nENDFILE\n";
    }
};

Soldank::Bullet CreateBullet(glm::vec2 position,
                             glm::vec2 velocity,
                             std::int16_t timeout = 100,
                             float hit_multiply = 1.0F)
{
    return Soldank::Bullet{ Soldank::BulletParams{
      .style = Soldank::BulletType::Bullet,
      .weapon = Soldank::WeaponType::Ak74,
      .position = position,
      .velocity = velocity,
      .timeout = timeout,
      .hit_multiply = hit_multiply,
      .team = Soldank::TeamType::None,
      .owner_id = 0,
      .push = 0.0F,
    } };
}

Soldank::StateManager CreateStateManager(Soldank::AnimationDataManager& animation_data_manager)
{
    return Soldank::StateManager(
      animation_data_manager,
      std::make_shared<Soldank::ParticleSystem>(std::vector<Soldank::Particle>{},
                                                std::vector<Soldank::Constraint>{}));
}

Soldank::StateManager CreateStateManagerWithSoldierSkeleton(
  Soldank::AnimationDataManager& animation_data_manager)
{
    std::vector<Soldank::Particle> particles;
    particles.reserve(23);
    for (int particle_index = 0; particle_index < 23; ++particle_index) {
        particles.emplace_back(true,
                               glm::vec2(0.0F, 0.0F),
                               glm::vec2(0.0F, 0.0F),
                               glm::vec2(0.0F, 0.0F),
                               glm::vec2(0.0F, 0.0F),
                               1.0F,
                               1.0F,
                               0.0F,
                               1.0F,
                               0.0F);
    }

    return Soldank::StateManager(
      animation_data_manager,
      std::make_shared<Soldank::ParticleSystem>(particles, std::vector<Soldank::Constraint>{}));
}

Soldank::AnimationDataManager CreateAnimationDataManager()
{
    AnimationDataReader animation_data_reader;
    Soldank::AnimationDataManager animation_data_manager;
    animation_data_manager.LoadAnimationData(
      Soldank::AnimationType::Stand, "stand.poa", true, 1, animation_data_reader);
    return animation_data_manager;
}
} // namespace

TEST(BulletPhysicsTest, MovesBulletUsingParticleEulerIntegration)
{
    auto animation_data_manager = CreateAnimationDataManager();
    auto state_manager = CreateStateManager(animation_data_manager);
    auto map = SoldankTesting::MapBuilder::Empty()->Build();
    Soldank::PhysicsEvents physics_events;
    auto bullet = CreateBullet({ 0.0F, 0.0F }, { 10.0F, 0.0F });

    Soldank::BulletPhysics::UpdateBullet(physics_events, bullet, *map, state_manager);

    EXPECT_EQ(bullet.particle.old_position, (glm::vec2{ 0.0F, 0.0F }));
    EXPECT_EQ(bullet.particle.position, (glm::vec2{ 10.0F, 0.135F }));
    EXPECT_EQ(bullet.particle.GetVelocity(), (glm::vec2{ 9.9F, 0.13365F }));
}

TEST(BulletPhysicsTest, DeactivatesBulletWhenItsLifetimeExpires)
{
    auto animation_data_manager = CreateAnimationDataManager();
    auto state_manager = CreateStateManager(animation_data_manager);
    auto map = SoldankTesting::MapBuilder::Empty()->Build();
    Soldank::PhysicsEvents physics_events;
    auto bullet = CreateBullet({ 0.0F, 0.0F }, { 0.0F, 0.0F }, 1);

    Soldank::BulletPhysics::UpdateBullet(physics_events, bullet, *map, state_manager);

    EXPECT_FALSE(bullet.active);
    EXPECT_EQ(bullet.timeout_prev, 1);
    EXPECT_EQ(bullet.timeout, 0);
}

TEST(BulletPhysicsTest, DeactivatesBulletOutsideMapBounds)
{
    auto animation_data_manager = CreateAnimationDataManager();
    auto state_manager = CreateStateManager(animation_data_manager);
    auto map = SoldankTesting::MapBuilder::Empty()->Build();
    Soldank::PhysicsEvents physics_events;
    auto bullet = CreateBullet({ 1'000.0F, 1'000.0F }, { 0.0F, 0.0F });

    Soldank::BulletPhysics::UpdateBullet(physics_events, bullet, *map, state_manager);

    EXPECT_FALSE(bullet.active);
}

TEST(BulletPhysicsTest, DetectsMapCollisionAlongBulletSweepAndEmitsEvent)
{
    auto animation_data_manager = CreateAnimationDataManager();
    auto state_manager = CreateStateManager(animation_data_manager);
    auto map =
      SoldankTesting::MapBuilder::Empty()
        ->AddPolygon(
          { -5.0F, -10.0F }, { 5.0F, -10.0F }, { 0.0F, 10.0F }, Soldank::PMSPolygonType::Normal)
        ->Build();
    Soldank::PhysicsEvents physics_events;
    int collision_count = 0;
    glm::vec2 collision_position{};
    physics_events.bullet_collides_with_polygon.AddObserver(
      [&](const Soldank::Bullet&, const glm::vec2& position) {
          ++collision_count;
          collision_position = position;
      });
    auto bullet = CreateBullet({ -20.0F, 0.0F }, { 40.0F, 0.0F });

    EXPECT_EQ(map->GetSectorsPerAxis(), 51);

    Soldank::BulletPhysics::UpdateBullet(physics_events, bullet, *map, state_manager);

    EXPECT_FALSE(bullet.active);
    EXPECT_EQ(collision_count, 1);
    EXPECT_LT(collision_position.x, 0.0F);
    EXPECT_GT(collision_position.y, 0.0F);
    EXPECT_LT(collision_position.y, 0.135F);
}

TEST(BulletPhysicsTest, MapCollisionResultContainsImpactDistanceAndTargetMetadata)
{
    auto map =
      SoldankTesting::MapBuilder::Empty()
        ->AddPolygon(
          { -5.0F, -10.0F }, { 5.0F, -10.0F }, { 0.0F, 10.0F }, Soldank::PMSPolygonType::Normal)
        ->Build();
    auto bullet = CreateBullet({ -20.0F, 0.0F }, { 40.0F, 0.0F });

    bullet.particle.Euler();
    const auto collision = Soldank::BulletCollision::FindMapCollision(bullet, *map);

    ASSERT_TRUE(collision.has_value());
    EXPECT_EQ(collision->kind, Soldank::BulletCollisionKind::MapPolygon);
    EXPECT_GT(collision->distance, 0.0F);
    EXPECT_TRUE(collision->polygon_id.has_value());
    EXPECT_EQ(*collision->polygon_id, 0U);
    EXPECT_FALSE(collision->soldier_id.has_value());
    EXPECT_FALSE(collision->body_part_id.has_value());
}

TEST(BulletPhysicsTest, SoldierCollisionResultContainsBodyPartAndImpactPosition)
{
    auto animation_data_manager = CreateAnimationDataManager();
    auto state_manager = CreateStateManagerWithSoldierSkeleton(animation_data_manager);
    auto soldiers = state_manager.CreateEmptySoldiersSnapshot();
    soldiers.at(0).active = true;
    state_manager.ApplySoldiersSnapshot(soldiers);
    state_manager.TransformSoldier(0, [](Soldank::Soldier& soldier) {
        soldier.dead_meat = true;
        soldier.particle.position = { 0.0F, 0.0F };
        for (unsigned int body_part_id = 1; body_part_id <= 16; ++body_part_id) {
            soldier.skeleton->SetPos(body_part_id, { 0.0F, 0.0F });
        }
    });
    auto bullet = CreateBullet({ -10.0F, 0.0F }, { 20.0F, 0.0F });
    bullet.particle.Euler();

    const auto collision =
      Soldank::BulletCollision::FindSoldierCollision(bullet, state_manager, -1.0F);

    ASSERT_TRUE(collision.has_value());
    EXPECT_EQ(collision->kind, Soldank::BulletCollisionKind::Soldier);
    EXPECT_EQ(collision->soldier_id, 0);
    EXPECT_EQ(collision->body_part_id, 12);
    EXPECT_NEAR(collision->position.x, -7.0F, 0.01F);
    EXPECT_NEAR(collision->position.y, 0.02F, 0.01F);
    EXPECT_NEAR(collision->distance, 3.0F, 0.01F);
}

TEST(BulletPhysicsTest, SelectsNearestSoldierCollisionRatherThanFirstActiveSoldier)
{
    auto animation_data_manager = CreateAnimationDataManager();
    auto state_manager = CreateStateManagerWithSoldierSkeleton(animation_data_manager);
    auto soldiers = state_manager.CreateEmptySoldiersSnapshot();
    soldiers.at(0).active = true;
    soldiers.at(1).active = true;
    state_manager.ApplySoldiersSnapshot(soldiers);

    const auto position_skeleton = [](Soldank::Soldier& soldier, glm::vec2 position) {
        soldier.dead_meat = true;
        soldier.particle.position = position;
        soldier.skeleton = std::make_shared<Soldank::ParticleSystem>(
          std::vector<Soldank::Particle>(
            23, Soldank::Particle(true, position, position, {}, {}, 1.0F, 1.0F, 0.0F, 1.0F, 0.0F)),
          std::vector<Soldank::Constraint>{});
        for (unsigned int body_part_id = 1; body_part_id <= 16; ++body_part_id) {
            soldier.skeleton->SetPos(body_part_id, position);
        }
    };
    state_manager.TransformSoldier(
      0, [&](Soldank::Soldier& soldier) { position_skeleton(soldier, { 20.0F, 0.0F }); });
    state_manager.TransformSoldier(
      1, [&](Soldank::Soldier& soldier) { position_skeleton(soldier, { 0.0F, 0.0F }); });
    auto bullet = CreateBullet({ -10.0F, 0.0F }, { 40.0F, 0.0F });
    bullet.particle.Euler();

    const auto collision =
      Soldank::BulletCollision::FindSoldierCollision(bullet, state_manager, -1.0F);

    ASSERT_TRUE(collision.has_value());
    EXPECT_EQ(collision->soldier_id, 1);
    EXPECT_LT(collision->distance, 20.0F);
}

TEST(BulletPhysicsTest, ResolvesTheNearestImpactAcrossMapAndSoldiers)
{
    auto animation_data_manager = CreateAnimationDataManager();
    auto state_manager = CreateStateManagerWithSoldierSkeleton(animation_data_manager);
    auto soldiers = state_manager.CreateEmptySoldiersSnapshot();
    soldiers.at(0).active = true;
    state_manager.ApplySoldiersSnapshot(soldiers);
    state_manager.TransformSoldier(0, [](Soldank::Soldier& soldier) {
        soldier.dead_meat = true;
        soldier.particle.position = { -5.0F, 0.0F };
        for (unsigned int body_part_id = 1; body_part_id <= 16; ++body_part_id) {
            soldier.skeleton->SetPos(body_part_id, { -5.0F, 0.0F });
        }
    });
    auto map =
      SoldankTesting::MapBuilder::Empty()
        ->AddPolygon(
          { 5.0F, -10.0F }, { 15.0F, -10.0F }, { 10.0F, 10.0F }, Soldank::PMSPolygonType::Normal)
        ->Build();
    Soldank::PhysicsEvents physics_events;
    int soldier_hit_count = 0;
    int map_hit_count = 0;
    physics_events.soldier_hit_by_bullet.AddObserver(
      [&](Soldank::Soldier&, float) { ++soldier_hit_count; });
    physics_events.bullet_collides_with_polygon.AddObserver(
      [&](const Soldank::Bullet&, const glm::vec2&) { ++map_hit_count; });
    auto bullet = CreateBullet({ -20.0F, 0.0F }, { 40.0F, 0.0F });

    Soldank::BulletPhysics::UpdateBullet(physics_events, bullet, *map, state_manager);

    EXPECT_EQ(soldier_hit_count, 1);
    EXPECT_EQ(map_hit_count, 0);
}

TEST(BulletPhysicsTest, MapBulletCollisionPolicyCoversEverySpecialPolygonType)
{
    constexpr std::array ALWAYS_COLLIDABLE{
        Soldank::PMSPolygonType::Normal,       Soldank::PMSPolygonType::Ice,
        Soldank::PMSPolygonType::Deadly,       Soldank::PMSPolygonType::BloodyDeadly,
        Soldank::PMSPolygonType::Hurts,        Soldank::PMSPolygonType::Regenerates,
        Soldank::PMSPolygonType::Lava,         Soldank::PMSPolygonType::Bouncy,
        Soldank::PMSPolygonType::Explosive,    Soldank::PMSPolygonType::HurtFlaggers,
        Soldank::PMSPolygonType::FlagCollides,
    };

    for (const auto polygon_type : ALWAYS_COLLIDABLE) {
        EXPECT_TRUE(Soldank::Map::BulletCollidesWithPolygon(polygon_type, 0));
    }

    EXPECT_TRUE(
      Soldank::Map::BulletCollidesWithPolygon(Soldank::PMSPolygonType::OnlyBulletsCollide, 0));
    EXPECT_FALSE(
      Soldank::Map::BulletCollidesWithPolygon(Soldank::PMSPolygonType::OnlyPlayersCollide, 0));
    EXPECT_FALSE(Soldank::Map::BulletCollidesWithPolygon(Soldank::PMSPolygonType::NoCollide, 0));
    EXPECT_FALSE(
      Soldank::Map::BulletCollidesWithPolygon(Soldank::PMSPolygonType::FlaggerCollides, 0));
    EXPECT_FALSE(
      Soldank::Map::BulletCollidesWithPolygon(Soldank::PMSPolygonType::NonFlaggerCollides, 0));
    EXPECT_FALSE(Soldank::Map::BulletCollidesWithPolygon(Soldank::PMSPolygonType::Background, 0));
    EXPECT_FALSE(
      Soldank::Map::BulletCollidesWithPolygon(Soldank::PMSPolygonType::BackgroundTransition, 0));

    constexpr std::array TEAM_BULLET_TYPES{
        Soldank::PMSPolygonType::AlphaBullets,
        Soldank::PMSPolygonType::BravoBullets,
        Soldank::PMSPolygonType::CharlieBullets,
        Soldank::PMSPolygonType::DeltaBullets,
    };
    constexpr std::array TEAM_PLAYER_TYPES{
        Soldank::PMSPolygonType::AlphaPlayers,
        Soldank::PMSPolygonType::BravoPlayers,
        Soldank::PMSPolygonType::CharliePlayers,
        Soldank::PMSPolygonType::DeltaPlayers,
    };

    for (std::uint8_t team_id = 1; team_id <= 4; ++team_id) {
        for (std::size_t type_index = 0; type_index < TEAM_BULLET_TYPES.size(); ++type_index) {
            EXPECT_EQ(
              Soldank::Map::BulletCollidesWithPolygon(TEAM_BULLET_TYPES.at(type_index), team_id),
              type_index == static_cast<std::size_t>(team_id - 1));
            EXPECT_FALSE(
              Soldank::Map::BulletCollidesWithPolygon(TEAM_PLAYER_TYPES.at(type_index), team_id));
        }
    }
}

TEST(BulletPhysicsTest, MapSweepIgnoresBulletExcludedPolygonTypes)
{
    constexpr std::array EXCLUDED_POLYGON_TYPES{
        Soldank::PMSPolygonType::OnlyPlayersCollide, Soldank::PMSPolygonType::NoCollide,
        Soldank::PMSPolygonType::FlaggerCollides,    Soldank::PMSPolygonType::NonFlaggerCollides,
        Soldank::PMSPolygonType::Background,         Soldank::PMSPolygonType::BackgroundTransition,
    };

    for (const auto polygon_type : EXCLUDED_POLYGON_TYPES) {
        auto map =
          SoldankTesting::MapBuilder::Empty()
            ->AddPolygon({ -5.0F, -10.0F }, { 5.0F, -10.0F }, { 0.0F, 10.0F }, polygon_type)
            ->Build();
        auto bullet = CreateBullet({ -20.0F, 0.0F }, { 40.0F, 0.0F });

        bullet.particle.Euler();

        EXPECT_FALSE(Soldank::BulletCollision::FindMapCollision(bullet, *map).has_value());
    }
}

TEST(BulletPhysicsTest, PreservesProjectilesForEligiblePenetratingHits)
{
    EXPECT_EQ(Soldank::BulletDamage::GetPenetrationVelocityMultiplier(true, true, 30.0F, 30.0F),
              0.9F);
    EXPECT_EQ(Soldank::BulletDamage::GetPenetrationVelocityMultiplier(false, true, 10.0F, 30.0F),
              0.75F);
    EXPECT_EQ(Soldank::BulletDamage::GetPenetrationVelocityMultiplier(false, false, 24.0F, 30.0F),
              0.75F);
    EXPECT_EQ(Soldank::BulletDamage::GetPenetrationVelocityMultiplier(false, false, 20.0F, 20.0F),
              0.66F);
    EXPECT_FALSE(
      Soldank::BulletDamage::GetPenetrationVelocityMultiplier(false, false, 10.0F, 30.0F));
}

TEST(BulletPhysicsTest, ReducesDamageMultiplierAfterTravellingPastFirstThreshold)
{
    auto animation_data_manager = CreateAnimationDataManager();
    auto state_manager = CreateStateManager(animation_data_manager);
    auto map = SoldankTesting::MapBuilder::Empty()->Build();
    Soldank::PhysicsEvents physics_events;
    auto bullet = CreateBullet({ 0.0F, 0.0F }, { 501.0F, 0.0F }, 7, 2.0F);

    Soldank::BulletPhysics::UpdateBullet(physics_events, bullet, *map, state_manager);

    EXPECT_EQ(bullet.degrade_count, 1U);
    EXPECT_FLOAT_EQ(bullet.hit_multiply_prev, 2.0F);
    EXPECT_FLOAT_EQ(bullet.hit_multiply, 1.0F);
}
