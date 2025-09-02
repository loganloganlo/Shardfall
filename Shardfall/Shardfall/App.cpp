#include "App.h"
#include "raylib.h"
#include "SaveSystem.h"   // bool LoadProfile(Profile&, const std::string&); bool SaveProfile(const Profile&, const std::string&);
#include "Classes.h"      // GetClassSpec, GetAbilitiesForClass
#include <cstdio>

// ------------------ Audio ------------------

void AppContext::InitAudio() {
    if (!IsAudioDeviceReady()) {
        InitAudioDevice();
    }
    audioReady = IsAudioDeviceReady();

    if (!audioReady) return;

    // Load assets if present (all optional)
    if (FileExists("assets/audio/menu_theme.ogg")) {
        menuMusic = LoadMusicStream("assets/audio/menu_theme.ogg");
    }
    if (FileExists("assets/audio/game_theme.ogg")) {
        gameMusic = LoadMusicStream("assets/audio/game_theme.ogg");
    }
    if (FileExists("assets/audio/ui_click.wav")) {
        clickSfx = LoadSound("assets/audio/ui_click.wav");
    }

    ApplyVolumes();
    // Start menu music on boot if present
    EnsureMenuMusic();
}

void AppContext::UpdateAudio(float /*dt*/) {
    if (!audioReady) return;
    if (menuMusic.stream.buffer) UpdateMusicStream(menuMusic);
    if (gameMusic.stream.buffer) UpdateMusicStream(gameMusic);
}

void AppContext::ShutdownAudio() {
    if (!audioReady) return;

    if (menuMusic.stream.buffer) UnloadMusicStream(menuMusic);
    if (gameMusic.stream.buffer) UnloadMusicStream(gameMusic);
    if (clickSfx.frameCount)     UnloadSound(clickSfx);

    CloseAudioDevice();
    audioReady = false;
}

void AppContext::EnsureMenuMusic() {
    if (!audioReady || !menuMusic.stream.buffer) return;
    // stop other
    if (gameMusic.stream.buffer) StopMusicStream(gameMusic);
    // (re)start menu
    PlayMusicStream(menuMusic);
    SetMusicVolume(menuMusic, profile.musicVolume);
}

void AppContext::EnsureGameMusic() {
    if (!audioReady || !gameMusic.stream.buffer) return;
    // stop other
    if (menuMusic.stream.buffer) StopMusicStream(menuMusic);
    // (re)start game
    PlayMusicStream(gameMusic);
    SetMusicVolume(gameMusic, profile.musicVolume);
}

void AppContext::StopAllMusic() {
    if (!audioReady) return;
    if (menuMusic.stream.buffer) StopMusicStream(menuMusic);
    if (gameMusic.stream.buffer) StopMusicStream(gameMusic);
}

void AppContext::PlayClick(float volMul) {
    if (!audioReady || !clickSfx.frameCount) return;
    SetSoundVolume(clickSfx, profile.sfxVolume * volMul);
    PlaySound(clickSfx);
}

void AppContext::ApplyVolumes() {
    if (!audioReady) return;
    if (menuMusic.stream.buffer) SetMusicVolume(menuMusic, profile.musicVolume);
    if (gameMusic.stream.buffer) SetMusicVolume(gameMusic, profile.musicVolume);
    if (clickSfx.frameCount)     SetSoundVolume(clickSfx, profile.sfxVolume);
}

// ------------------ Persistence ------------------

void AppContext::LoadProfile() {
    Profile p; // local

    // Try to load from disk (same folder as the exe/project)
    if (!LoadProfile(p, "profile.json")) {
        // Start with fresh defaults if missing/corrupt
        p = Profile{};
    }

    // Copy into app state
    profile = p;

    // Build sensible defaults & consistency clamps
    {
        // All abilities available for the chosen class
        const std::vector<AbilityId> allowed = GetAbilitiesForClass(profile.cls);

        // If nothing unlocked yet, unlock all class abilities as a starter profile
        if (profile.unlocked.empty()) {
            profile.unlocked = allowed;
        }

        // If nothing equipped yet, use class defaults (cap to 6)
        if (profile.equipped.empty()) {
            const auto& spec = GetClassSpec(profile.cls);
            profile.equipped = spec.defaultEquipped;
            if (profile.equipped.size() > 6) profile.equipped.resize(6);
        }

        // Final consistency clamp (volumes, dedupe, filter to allowed/unlocked, cap to 6…)
        profile.EnsureValidity();
    }

    // Reflect volume settings into audio
    ApplyVolumes();
}

void AppContext::SaveProfile() {
    // Make sure the profile is internally consistent before saving
    profile.EnsureValidity();
    SaveProfile(profile, "profile.json");
}
