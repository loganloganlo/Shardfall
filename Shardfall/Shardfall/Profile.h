#pragma once
#include <string>
#include <vector>
#include "Data.h"

// Central player profile saved/loaded by SaveSystem
struct Profile {
    // Basic identity (optional, but your code expects it)
    std::string name = "Player";

    // Class choice
    ClassId cls = ClassId::Warrior;

    // Ability progression
    std::vector<AbilityId> unlocked;  // abilities the player has unlocked overall
    std::vector<AbilityId> equipped;  // up to 6 currently equipped

    // Settings
    float musicVolume = 0.8f; // 0..1
    float sfxVolume = 0.9f; // 0..1

    // Ensure internal consistency (used by SaveSystem/App)
    void EnsureValidity();
};
