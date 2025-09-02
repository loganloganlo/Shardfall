#include "Data.h"
#include <algorithm>

static bool ieq(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        char ca = (char)tolower(a[i]);
        char cb = (char)tolower(b[i]);
        if (ca != cb) return false;
    }
    return true;
}

const char* ToString(ClassId id) {
    switch (id) {
    case ClassId::Warrior: return "Warrior";
    case ClassId::Mage:    return "Mage";
    case ClassId::Rogue:   return "Rogue";
    }
    return "(unknown class)";
}
bool FromString(const std::string& s, ClassId& out) {
    if (ieq(s, "Warrior")) { out = ClassId::Warrior; return true; }
    if (ieq(s, "Mage")) { out = ClassId::Mage;    return true; }
    if (ieq(s, "Rogue")) { out = ClassId::Rogue;   return true; }
    return false;
}

const char* ToString(AbilityId id) {
    switch (id) {
    case AbilityId::None:                return "(none)";

        // Warrior
    case AbilityId::Warrior_Cleave:      return "Cleave";
    case AbilityId::Warrior_Whirlwind:   return "Whirlwind";
    case AbilityId::Warrior_ShieldBash:  return "Shield Bash";
    case AbilityId::Warrior_GroundSlam:  return "Ground Slam";
    case AbilityId::Warrior_Charge:      return "Charge";
    case AbilityId::Warrior_Shockwave:   return "Shockwave";
    case AbilityId::Warrior_BattleShout: return "Battle Shout";
    case AbilityId::Warrior_Leap:        return "Leap";
    case AbilityId::Warrior_Taunt:       return "Taunt";
    case AbilityId::Warrior_PowerStrike: return "Power Strike";
    case AbilityId::Warrior_EarthSplit:  return "Earth Splitter";
    case AbilityId::Warrior_Rally:       return "Rally";
    case AbilityId::Warrior_Parry:       return "Parry";
    case AbilityId::Warrior_Berserk:     return "Berserk";

        // Mage
    case AbilityId::Mage_Fireball:       return "Fireball";
    case AbilityId::Mage_IceSpike:       return "Ice Spike";
    case AbilityId::Mage_ArcaneNova:     return "Arcane Nova";
    case AbilityId::Mage_FrostNova:      return "Frost Nova";
    case AbilityId::Mage_Blink:          return "Blink";
    case AbilityId::Mage_ChainBolt:      return "Chain Bolt";
    case AbilityId::Mage_Meteor:         return "Meteor";
    case AbilityId::Mage_SlowField:      return "Slow Field";
    case AbilityId::Mage_MagicMissiles:  return "Magic Missiles";
    case AbilityId::Mage_FlameWave:      return "Flame Wave";
    case AbilityId::Mage_Prisms:         return "Prism Shot";
    case AbilityId::Mage_Spark:          return "Spark";
    case AbilityId::Mage_RuneBomb:       return "Rune Bomb";
    case AbilityId::Mage_BlizzardBurst:  return "Blizzard";

        // Rogue
    case AbilityId::Rogue_DaggerThrow:   return "Dagger Throw";
    case AbilityId::Rogue_FanKnives:     return "Fan of Knives";
    case AbilityId::Rogue_DashStrike:    return "Dash Strike";
    case AbilityId::Rogue_SmokeBomb:     return "Smoke Bomb";
    case AbilityId::Rogue_PoisonCloud:   return "Poison Cloud";
    case AbilityId::Rogue_Trap:          return "Trap";
    case AbilityId::Rogue_Shadowstep:    return "Shadowstep";
    case AbilityId::Rogue_Backstab:      return "Backstab";
    case AbilityId::Rogue_Shiv:          return "Shiv";
    case AbilityId::Rogue_Shuriken:      return "Shuriken Volley";
    case AbilityId::Rogue_Net:           return "Net";
    case AbilityId::Rogue_Cripple:       return "Cripple";
    case AbilityId::Rogue_ExplosiveVial: return "Explosive Vial";
    case AbilityId::Rogue_DecoyBurst:    return "Decoy Burst";
    }
    return "(unknown ability)";
}

bool FromString(const std::string& s, AbilityId& out) {
    // quick map by case-insensitive compare to ToString()
#define TRY(name) if (ieq(s, ToString(AbilityId::name))) { out = AbilityId::name; return true; }
    if (ieq(s, "(none)")) { out = AbilityId::None; return true; }

    // Warrior
    TRY(Warrior_Cleave) TRY(Warrior_Whirlwind) TRY(Warrior_ShieldBash)
        TRY(Warrior_GroundSlam) TRY(Warrior_Charge) TRY(Warrior_Shockwave)
        TRY(Warrior_BattleShout) TRY(Warrior_Leap) TRY(Warrior_Taunt)
        TRY(Warrior_PowerStrike) TRY(Warrior_EarthSplit) TRY(Warrior_Rally)
        TRY(Warrior_Parry) TRY(Warrior_Berserk)

        // Mage
        TRY(Mage_Fireball) TRY(Mage_IceSpike) TRY(Mage_ArcaneNova) TRY(Mage_FrostNova)
        TRY(Mage_Blink) TRY(Mage_ChainBolt) TRY(Mage_Meteor) TRY(Mage_SlowField)
        TRY(Mage_MagicMissiles) TRY(Mage_FlameWave) TRY(Mage_Prisms) TRY(Mage_Spark)
        TRY(Mage_RuneBomb) TRY(Mage_BlizzardBurst)

        // Rogue
        TRY(Rogue_DaggerThrow) TRY(Rogue_FanKnives) TRY(Rogue_DashStrike) TRY(Rogue_SmokeBomb)
        TRY(Rogue_PoisonCloud) TRY(Rogue_Trap) TRY(Rogue_Shadowstep) TRY(Rogue_Backstab)
        TRY(Rogue_Shiv) TRY(Rogue_Shuriken) TRY(Rogue_Net) TRY(Rogue_Cripple)
        TRY(Rogue_ExplosiveVial) TRY(Rogue_DecoyBurst)

#undef TRY
        return false;
}
