#include "core/entities/Weapon.hpp"
#include "core/entities/WeaponParameters.hpp"

#include <gtest/gtest.h>

TEST(WeaponTests, TestWeaponsLoadedCorrectly)
{
    Soldank::WeaponParameters weapon_parameters;
    weapon_parameters.ammo = 50;
    Soldank::Weapon weapon(weapon_parameters);
    ASSERT_EQ(weapon.GetAmmoCount(), 50);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}