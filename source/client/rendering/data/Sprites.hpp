#ifndef __SPRITES_HPP__
#define __SPRITES_HPP__

#include "core/math/Glm.hpp"

#include <string>
#include <vector>
#include <filesystem>
#include <optional>

namespace Soldat::Sprites
{
enum class SoldierPartType : unsigned int
{
    Stopa = 0,
    Stopa2,
    Noga,
    Noga2,
    Udo,
    Udo2,
    Biodro,
    Biodro2,
    Klata,
    Klata2,
    Morda,
    Morda2,
    Ramie,
    Ramie2,
    Reka,
    Reka2,
    Dlon,
    Dlon2,
    Lancuch,
    Helm,
    Helm2,
    Badge,
    Badge2,
    Cygaro,
    Cygaro2,
    Metal,
    Metal2,
    Zloto,
    Zloto2,
    Zlotylancuch,
    Kap,
    Kap2,
    Dred,
    Hair1,
    Hair12,
    Hair2,
    Hair22,
    Hair3,
    Hair32,
    Hair4,
    Hair42,
    Kamizelka,
    Kamizelka2,
    RannyKlata,
    RannyKlata2,
    RannyMorda,
    RannyMorda2,
    RannyNoga,
    RannyNoga2,
    RannyRamie,
    RannyRamie2,
    RannyReka,
    RannyReka2,
    RannyUdo,
    RannyUdo2,
    RannyBiodro,
    RannyBiodro2,
    Lecistopa,
    Lecistopa2,

    // Soldier Team2 list. keep it next to the non-team2 list.

    Team2Stopa,
    Team2Stopa2,
    Team2Noga,
    Team2Noga2,
    Team2Udo,
    Team2Udo2,
    Team2Biodro,
    Team2Biodro2,
    Team2Klata,
    Team2Klata2,
    Team2Morda,
    Team2Morda2,
    Team2Ramie,
    Team2Ramie2,
    Team2Reka,
    Team2Reka2,
    Team2Dlon,
    Team2Dlon2,
    Team2Lancuch,
    Team2Helm,
    Team2Helm2,
    Team2Badge,
    Team2Badge2,
    Team2Cygaro,
    Team2Cygaro2,
    Team2Metal,
    Team2Metal2,
    Team2Zloto,
    Team2Zloto2,
    Team2Zlotylancuch,
    Team2Kap,
    Team2Kap2,
    Team2Dred,
    Team2Hair1,
    Team2Hair12,
    Team2Hair2,
    Team2Hair22,
    Team2Hair3,
    Team2Hair32,
    Team2Hair4,
    Team2Hair42,
    Team2Kamizelka,
    Team2Kamizelka2,
    Team2RannyKlata,
    Team2RannyKlata2,
    Team2RannyMorda,
    Team2RannyMorda2,
    Team2RannyNoga,
    Team2RannyNoga2,
    Team2RannyRamie,
    Team2RannyRamie2,
    Team2RannyReka,
    Team2RannyReka2,
    Team2RannyUdo,
    Team2RannyUdo2,
    Team2RannyBiodro,
    Team2RannyBiodro2,
    Team2Lecistopa,
    Team2Lecistopa2,

    // Preserve order of these parachute textures.

    ParaRope,
    Para,
    Para2
};

// The range Shell to M2Stat is checked for size restriction too, so
// keep that range together.

enum class WeaponType : unsigned int
{
    Shell = 0,
    Bullet,
    Smudge,
    Missile,
    Cluster,
    ClusterGrenade,
    FragGrenade,
    Ak74,
    Ak742,
    Ak74Clip,
    Ak74Clip2,
    Ak74Shell,
    Ak74Bullet,
    Ak74Fire,
    Minimi,
    Minimi2,
    MinimiClip,
    MinimiClip2,
    MinimiShell,
    MinimiBullet,
    MinimiFire,
    Ruger,
    Ruger2,
    RugerClip,
    RugerShell,
    RugerBullet,
    RugerFire,
    Mp5,
    Mp52,
    Mp5Clip,
    Mp5Clip2,
    Mp5Shell,
    Mp5Bullet,
    Mp5Fire,
    Spas,
    Spas2,
    SpasClip,
    SpasShell,
    SpasBullet,
    SpasFire,
    M79,
    M792,
    M79Clip,
    M79Clip2,
    M79Shell,
    M79Bullet,
    M79Fire,
    Deagles,
    Deagles2,
    NDeagles,
    NDeagles2,
    DeaglesClip,
    DeaglesClip2,
    DeaglesShell,
    DeaglesBullet,
    DeaglesFire,
    Steyr,
    Steyr2,
    SteyrClip,
    SteyrClip2,
    SteyrShell,
    SteyrBullet,
    SteyrFire,
    Barrett,
    Barrett2,
    BarrettClip,
    BarrettClip2,
    BarrettShell,
    BarrettBullet,
    BarrettFire,
    Minigun,
    Minigun2,
    MinigunClip,
    MinigunShell,
    MinigunBullet,
    MinigunFire,
    Socom,
    Socom2,
    NSocom,
    NSocom2,
    SocomClip,
    SocomClip2,
    ColtShell,
    ColtBullet,
    SocomFire,
    Bow,
    Bow2,
    BowS,
    BowA,
    NBow,
    NBow2,
    Arrow,
    BowFire,
    Flamer,
    Flamer2,
    FlamerClip,
    FlamerFire,
    Knife,
    Knife2,
    KnifeClip,
    KnifeFire,
    Chainsaw,
    Chainsaw2,
    ChainsawClip,
    ChainsawFire,
    Law,
    Law2,
    LawClip,
    LawFire,
    M2,
    M22,
    M2Stat,
};

// Preserve order of:
// - ExplosionExplode1 to ExplosionExplode16
// - ExplosionSmoke1 to Minismoke
// - FlamesExplode1 to FlamesExplode16

enum class SparkType : unsigned int
{
    Smoke = 0,
    Lilfire,
    Odprysk,
    Blood,
    ExplosionExplode1,
    ExplosionExplode2,
    ExplosionExplode3,
    ExplosionExplode4,
    ExplosionExplode5,
    ExplosionExplode6,
    ExplosionExplode7,
    ExplosionExplode8,
    ExplosionExplode9,
    ExplosionExplode10,
    ExplosionExplode11,
    ExplosionExplode12,
    ExplosionExplode13,
    ExplosionExplode14,
    ExplosionExplode15,
    ExplosionExplode16,
    Bigsmoke,
    Spawnspark,
    Pin,
    Lilsmoke,
    Stuff,
    Cygaro,
    Plomyk,
    Blacksmoke,
    Rain,
    Sand,
    Odlamek1,
    Odlamek2,
    Odlamek3,
    Odlamek4,
    Skrawek,
    Puff,
    Snow,
    ExplosionSmoke1,
    ExplosionSmoke2,
    ExplosionSmoke3,
    ExplosionSmoke4,
    ExplosionSmoke5,
    ExplosionSmoke6,
    ExplosionSmoke7,
    ExplosionSmoke8,
    ExplosionSmoke9,
    Splat,
    Minismoke,
    Bigsmoke2,
    Jetfire,
    Lilblood,
    FlamesExplode1,
    FlamesExplode2,
    FlamesExplode3,
    FlamesExplode4,
    FlamesExplode5,
    FlamesExplode6,
    FlamesExplode7,
    FlamesExplode8,
    FlamesExplode9,
    FlamesExplode10,
    FlamesExplode11,
    FlamesExplode12,
    FlamesExplode13,
    FlamesExplode14,
    FlamesExplode15,
    FlamesExplode16,
};

enum class ObjectType : unsigned int
{
    Flag = 0,
    Infflag,
    Medikit,
    Grenadekit,
    Flamerkit,
    Predatorkit,
    Vestkit,
    Berserkerkit,
    Clusterkit,
    Ilum,
    FlagHandle,
};

// Preserve order of Guns*

enum class InterfaceType : unsigned int
{
    Sight = 0,
    GunsDeagles,
    GunsMp5,
    GunsAk74,
    GunsSteyr,
    GunsSpas,
    GunsRuger,
    GunsM79,
    GunsBarrett,
    GunsMinimi,
    GunsMinigun,
    GunsSocom,
    GunsKnife,
    GunsChainsaw,
    GunsLaw,
    GunsFlamer,
    GunsBow,
    GunsFist,
    GunsM2,
    Star,
    Smalldot,
    Deaddot,
    Mute,
    Prot,
    Scroll,
    Bot,
    Cursor,
    Health,
    Ammo,
    Jet,
    HealthBar,
    JetBar,
    ReloadBar,
    Back,
    Overlay,
    Nade,
    Noflag,
    Flag,
    ClusterNade,
    Dot,
    FireBar,
    FireBarR,
    VestBar,
    Menucursor,
    Arrow,
    TitleL,
    TitleR,
};

enum class SoldierColor : unsigned int
{
    None = 0,
    Main,
    Pants,
    Skin,
    Hair,
    Cygar,
    Headblood,
};

enum class SoldierAlpha : unsigned int
{
    Base = 0,
    Blood,
    Nades,
};

class SoldierPartData
{
public:
    SoldierPartData(const std::filesystem::path& file_path,
                    glm::uvec2 point,
                    glm::vec2 center,
                    bool visible,
                    std::optional<std::string> flipped_file_path,
                    bool team,
                    float flexibility,
                    SoldierColor color,
                    SoldierAlpha alpha);
    ~SoldierPartData();

    // it's not safe to be able to copy/move this because we would also need to take care of the
    // created OpenGL buffers and textures
    SoldierPartData(const SoldierPartData&) = delete;
    SoldierPartData& operator=(SoldierPartData other) = delete;
    SoldierPartData(SoldierPartData&&) = delete;
    SoldierPartData& operator=(SoldierPartData&& other) = delete;

    glm::uvec2 GetPoint() const { return point_; }
    glm::vec2 GetCenter() const { return center_; }

    bool IsVisible() const { return visible_; }
    bool IsFlippable() const { return flip_; }

    float GetFlexibility() const { return flexibility_; }

    unsigned int GetTexture() const { return texture_; }
    unsigned int GetTextureFlipped() const { return texture_flipped_; }

    int GetTextureWidth() const { return texture_width_; }
    int GetTextureHeight() const { return texture_height_; }

    int GetTextureFlippedWidth() const { return texture_flipped_width_; }
    int GetTextureFlippedHeight() const { return texture_flipped_height_; }

private:
    std::filesystem::path file_path_;
    glm::uvec2 point_;
    glm::vec2 center_;
    bool visible_;
    bool flip_;
    bool team_;
    float flexibility_;
    SoldierColor color_;
    SoldierAlpha alpha_;
    unsigned int texture_;
    unsigned int texture_flipped_;
    int texture_width_;
    int texture_height_;
    int texture_flipped_width_;
    int texture_flipped_height_;
};

void Init();
const SoldierPartData* Get(unsigned int id);
unsigned int GetSoldierPartCount();
// const SoldierPartData& Get(WeaponType weapon_type);
void Free();
} // namespace Soldat::Sprites

#endif
