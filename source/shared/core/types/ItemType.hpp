#ifndef __ITEM_TYPE_HPP__
#define __ITEM_TYPE_HPP__

#include <cstdint>

namespace Soldank
{
enum class ItemType : std::uint8_t
{
    AlphaFlag = 1,
    BravoFlag,
    PointmatchFlag,
    USSOCOM,
    DesertEagles,
    MP5,
    Ak74,
    SteyrAUG,
    Spas12,
    Ruger77,
    M79,
    Barrett,
    Minimi,
    Minigun,
    Bow,
    MedicalKit,
    GrenadeKit,
    FlamerKit,
    PredatorKit,
    VestKit,
    BerserkKit,
    ClusterKit,
    Parachute,
    Knife,
    Chainsaw,
    LAW,
    M2,
};

bool IsItemTypeFlag(ItemType item_type);
bool IsItemTypeWeapon(ItemType item_type);
bool IsItemTypeKit(ItemType item_type);
} // namespace Soldank

#endif
