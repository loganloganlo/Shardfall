#pragma once
#include "raylib.h"
#include <vector>
#include "Data.h"

struct ClassSpec {
    enum Shape { Square, Circle, Triangle };
    enum PrimaryKind { MeleeCone, Projectile };

    // visuals
    Shape shape = Square;
    Color color{ 255,255,255,255 };

    // movement
    float baseSpeed = 220.0f;

    // primary attack config
    PrimaryKind primaryKind = MeleeCone;

    // melee primary
    float primaryRange = 80.0f;
    float primaryArcDeg = 60.0f;
    float primaryDamage = 16.0f;

    // projectile primary
    float primaryProjSpeed = 900.0f;
    float primaryProjLife = 1.2f;
    float primaryProjRadius = 7.0f;
    float primaryProjDamage = 18.0f;
    Color primaryProjColor{ 230,220,255,255 };

    // defaults for loadout screen (6 max)
    std::vector<AbilityId> defaultEquipped;
};

const ClassSpec& GetClassSpec(ClassId id);

// loadout helpers
const ClassSpec* AllClassSpecs(size_t& outCount);
std::vector<AbilityId> GetAbilitiesForClass(ClassId id);
