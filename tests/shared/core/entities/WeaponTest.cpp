#include <gtest/gtest.h>

#include <cstdint>

import Shared.Core.Entities.Weapon;
import Shared.Core.Entities.WeaponParameters;
import Shared.Core.Entities.Soldier;
import Shared.Core.Types.TeamType;

TEST(WeaponTests, TestWeaponsLoadedCorrectly)
{
    Soldank::WeaponParameters weapon_parameters;
    weapon_parameters.ammo = 50;
    Soldank::Weapon weapon(weapon_parameters);
    ASSERT_EQ(weapon.GetAmmoCount(), 50);
}

TEST(WeaponTests, ExposesOpenSoldatNoCollisionMasks)
{
    constexpr std::uint8_t collision_mask =
      static_cast<std::uint8_t>(Soldank::WeaponNoCollision::Enemy) |
      static_cast<std::uint8_t>(Soldank::WeaponNoCollision::ExplosionSelf);

    EXPECT_TRUE(Soldank::HasWeaponNoCollision(collision_mask, Soldank::WeaponNoCollision::Enemy));
    EXPECT_FALSE(Soldank::HasWeaponNoCollision(collision_mask, Soldank::WeaponNoCollision::Team));
    EXPECT_TRUE(
      Soldank::HasWeaponNoCollision(collision_mask, Soldank::WeaponNoCollision::ExplosionSelf));
}

TEST(WeaponTests, ExposesCombatStateThroughSoldierApis)
{
    Soldank::WeaponParameters weapon_parameters{};
    weapon_parameters.ammo = 10;
    Soldank::Soldier soldier;
    soldier.active = true;
    soldier.dead_meat = false;
    soldier.weapons.emplace_back(weapon_parameters);

    soldier.SetTeam(Soldank::TeamType::Alpha);
    soldier.SetCeaseFireCounter(-1);
    soldier.SetPingTicks(12);

    EXPECT_EQ(soldier.GetTeam(), Soldank::TeamType::Alpha);
    EXPECT_FALSE(soldier.IsSpectator());
    EXPECT_TRUE(soldier.IsAlive());
    EXPECT_FALSE(soldier.IsRagdoll());
    EXPECT_TRUE(soldier.CanReceiveCombatImpact());
    EXPECT_EQ(soldier.GetPingTicks(), 12);
    EXPECT_EQ(soldier.GetActiveWeapon().GetAmmoCount(), 10);

    soldier.SetTeam(Soldank::TeamType::Spectator);
    EXPECT_TRUE(soldier.IsSpectator());
    EXPECT_FALSE(soldier.CanReceiveCombatImpact());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
