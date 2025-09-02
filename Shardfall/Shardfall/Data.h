#pragma once
#include <cstdint>
#include <string>

// ----- Classes -----
enum class ClassId : uint32_t {
    Warrior = 0,
    Mage,
    Rogue
};

const char* ToString(ClassId id);
bool FromString(const std::string& s, ClassId& out);

// ----- Abilities (42: 14 per class) -----
enum class AbilityId : uint32_t {
    None = 0,

    // Warrior (14)
    Warrior_Cleave,
    Warrior_Whirlwind,
    Warrior_ShieldBash,
    Warrior_GroundSlam,
    Warrior_Charge,
    Warrior_Shockwave,
    Warrior_BattleShout,
    Warrior_Leap,
    Warrior_Taunt,
    Warrior_PowerStrike,
    Warrior_EarthSplit,
    Warrior_Rally,
    Warrior_Parry,
    Warrior_Berserk,

    // Mage (14)
    Mage_Fireball,
    Mage_IceSpike,
    Mage_ArcaneNova,
    Mage_FrostNova,
    Mage_Blink,
    Mage_ChainBolt,
    Mage_Meteor,
    Mage_SlowField,
    Mage_MagicMissiles,
    Mage_FlameWave,
    Mage_Prisms,
    Mage_Spark,
    Mage_RuneBomb,
    Mage_BlizzardBurst,

    // Rogue (14)
    Rogue_DaggerThrow,
    Rogue_FanKnives,
    Rogue_DashStrike,
    Rogue_SmokeBomb,
    Rogue_PoisonCloud,
    Rogue_Trap,
    Rogue_Shadowstep,
    Rogue_Backstab,
    Rogue_Shiv,
    Rogue_Shuriken,
    Rogue_Net,
    Rogue_Cripple,
    Rogue_ExplosiveVial,
    Rogue_DecoyBurst,
};

const char* ToString(AbilityId id);
bool FromString(const std::string& s, AbilityId& out);

// NOTE: Intentionally NO Profile here.
// NOTE: Intentionally NO SceneRequest here.
