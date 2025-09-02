#pragma once
#include "raylib.h"
#include <string>
#include "Profile.h"   // Profile definition (name, unlocked, equipped, volumes)

struct AppContext {
    // Persistent player profile (saved/loaded on disk)
    Profile profile;

    // --- Audio ---
    bool  audioReady = false;
    Music menuMusic{};
    Music gameMusic{};
    Sound clickSfx{};

    // Lifecycle
    void InitAudio();
    void UpdateAudio(float dt);
    void ShutdownAudio();

    // Music control helpers
    void EnsureMenuMusic();
    void EnsureGameMusic();
    void StopAllMusic();

    // SFX
    void PlayClick(float volMul = 1.0f);

    // Volumes
    void ApplyVolumes();

    // Persistence
    void LoadProfile();   // loads profile.json (and fills sane defaults if missing)
    void SaveProfile();   // saves profile.json
};
