#include "Classes.h"

static ClassSpec BuildWarrior() {
    ClassSpec s;
    s.shape = ClassSpec::Square;
    s.color = Color{ 220,60,60,255 };
    s.baseSpeed = 220.0f;
    s.primaryKind = ClassSpec::MeleeCone;
    s.primaryRange = 95.0f;
    s.primaryArcDeg = 80.0f;
    s.primaryDamage = 22.0f;

    s.defaultEquipped = {
        AbilityId::Warrior_Cleave,
        AbilityId::Warrior_ShieldBash,
        AbilityId::Warrior_Charge,
        AbilityId::Warrior_Whirlwind,
        AbilityId::Warrior_PowerStrike,
        AbilityId::Warrior_GroundSlam
    };
    return s;
}
static ClassSpec BuildMage() {
    ClassSpec s;
    s.shape = ClassSpec::Circle;
    s.color = Color{ 70,140,255,255 };
    s.baseSpeed = 230.0f;
    s.primaryKind = ClassSpec::Projectile;
    s.primaryProjSpeed = 920.0f;
    s.primaryProjLife = 1.2f;
    s.primaryProjRadius = 8.0f;
    s.primaryProjDamage = 20.0f;
    s.primaryProjColor = Color{ 210,210,255,255 };

    s.defaultEquipped = {
        AbilityId::Mage_Fireball,
        AbilityId::Mage_IceSpike,
        AbilityId::Mage_Blink,
        AbilityId::Mage_ArcaneNova,
        AbilityId::Mage_MagicMissiles,
        AbilityId::Mage_FrostNova
    };
    return s;
}
static ClassSpec BuildRogue() {
    ClassSpec s;
    s.shape = ClassSpec::Triangle;
    s.color = Color{ 245,230,90,255 };
    s.baseSpeed = 250.0f;
    s.primaryKind = ClassSpec::MeleeCone;
    s.primaryRange = 70.0f;
    s.primaryArcDeg = 70.0f;
    s.primaryDamage = 18.0f;

    s.defaultEquipped = {
        AbilityId::Rogue_DaggerThrow,
        AbilityId::Rogue_DashStrike,
        AbilityId::Rogue_FanKnives,
        AbilityId::Rogue_SmokeBomb,
        AbilityId::Rogue_Backstab,
        AbilityId::Rogue_Shuriken
    };
    return s;
}

const ClassSpec& GetClassSpec(ClassId id) {
    static ClassSpec WARRIOR = BuildWarrior();
    static ClassSpec MAGE = BuildMage();
    static ClassSpec ROGUE = BuildRogue();

    switch (id) {
    case ClassId::Warrior: return WARRIOR;
    case ClassId::Mage:    return MAGE;
    case ClassId::Rogue:   return ROGUE;
    }
    return WARRIOR;
}

const ClassSpec* AllClassSpecs(size_t& outCount) {
    static ClassSpec W = BuildWarrior();
    static ClassSpec M = BuildMage();
    static ClassSpec R = BuildRogue();
    static ClassSpec* arr[3] = { &W, &M, &R };
    outCount = 3;
    return arr[0];
}

std::vector<AbilityId> GetAbilitiesForClass(ClassId id) {
    switch (id) {
    case ClassId::Warrior: return {
        AbilityId::Warrior_Cleave, AbilityId::Warrior_Whirlwind, AbilityId::Warrior_ShieldBash,
        AbilityId::Warrior_GroundSlam, AbilityId::Warrior_Charge, AbilityId::Warrior_Shockwave,
        AbilityId::Warrior_BattleShout, AbilityId::Warrior_Leap, AbilityId::Warrior_Taunt,
        AbilityId::Warrior_PowerStrike, AbilityId::Warrior_EarthSplit, AbilityId::Warrior_Rally,
        AbilityId::Warrior_Parry, AbilityId::Warrior_Berserk
    };
    case ClassId::Mage: return {
        AbilityId::Mage_Fireball, AbilityId::Mage_IceSpike, AbilityId::Mage_ArcaneNova,
        AbilityId::Mage_FrostNova, AbilityId::Mage_Blink, AbilityId::Mage_ChainBolt,
        AbilityId::Mage_Meteor, AbilityId::Mage_SlowField, AbilityId::Mage_MagicMissiles,
        AbilityId::Mage_FlameWave, AbilityId::Mage_Prisms, AbilityId::Mage_Spark,
        AbilityId::Mage_RuneBomb, AbilityId::Mage_BlizzardBurst
    };
    case ClassId::Rogue: return {
        AbilityId::Rogue_DaggerThrow, AbilityId::Rogue_FanKnives, AbilityId::Rogue_DashStrike,
        AbilityId::Rogue_SmokeBomb, AbilityId::Rogue_PoisonCloud, AbilityId::Rogue_Trap,
        AbilityId::Rogue_Shadowstep, AbilityId::Rogue_Backstab, AbilityId::Rogue_Shiv,
        AbilityId::Rogue_Shuriken, AbilityId::Rogue_Net, AbilityId::Rogue_Cripple,
        AbilityId::Rogue_ExplosiveVial, AbilityId::Rogue_DecoyBurst
    };
    }
    return {};
}
