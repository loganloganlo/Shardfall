#include "Audio.h"
#include <algorithm>
#include <vector>
#include <string>

static inline float clamp01(float v) {
    return (v < 0.0f) ? 0.0f : (v > 1.0f ? 1.0f : v);
}

std::string Audio::FindAsset(const char* filename) {
    std::vector<std::string> tries = {
        filename,
        std::string("assets/") + filename,
        std::string("../assets/") + filename,
        std::string("../../assets/") + filename
    };
    for (auto& p : tries) if (FileExists(p.c_str())) return p;
    return std::string();
}

std::string Audio::FirstExisting(const char* const* names, int count) {
    for (int i = 0; i < count; ++i) {
        auto p = FindAsset(names[i]);
        if (!p.empty()) return p;
    }
    return std::string();
}

void Audio::Init(const std::string& /*assetDir*/) {
    if (!IsAudioDeviceReady()) InitAudioDevice();

    // Try multiple common names for menu music
    const char* menuNames[] = { "menu.ogg", "menu_theme.ogg", "menu_theme.mp3", "menu_theme.wav", "menu_loop.ogg" };
    const char* gameNames[] = { "game.ogg", "game_theme.ogg", "gameplay.ogg", "arena.ogg" };
    const char* clickNames[] = { "ui_click.wav", "click.wav", "ui_click.ogg" };

    std::string menuPath = FirstExisting(menuNames, (int)(sizeof(menuNames) / sizeof(menuNames[0])));
    std::string gamePath = FirstExisting(gameNames, (int)(sizeof(gameNames) / sizeof(gameNames[0])));
    std::string clickPath = FirstExisting(clickNames, (int)(sizeof(clickNames) / sizeof(clickNames[0])));

    if (!menuPath.empty()) { musicMenu = LoadMusicStream(menuPath.c_str()); hasMenu = (musicMenu.ctxData != nullptr); }
    if (!gamePath.empty()) { musicGame = LoadMusicStream(gamePath.c_str()); hasGame = (musicGame.ctxData != nullptr); }
    if (!clickPath.empty()) { uiClick = LoadSound(clickPath.c_str()); hasClick = (uiClick.frameCount > 0); }

    // Default to menu if available
    if (hasMenu) {
        cur = &musicMenu; curTrack = MusicTrack::Menu;
        SafeStart(cur);
        SafeSet(cur, volMaster * volMusic);
    }
}

void Audio::SetVolumes(float master, float music, float sfx) {
    volMaster = clamp01(master);
    volMusic = clamp01(music);
    volSfx = clamp01(sfx);

    if (cur) SafeSet(cur, volMaster * volMusic);
    if (nxt) SafeSet(nxt, 0.0f); // next begins from 0 and fades in
}

void Audio::SetTrack(MusicTrack t, float crossfadeSec) {
    if (t == curTrack || t == MusicTrack::None) return;

    // Select candidate
    Music* candidate = nullptr;
    if (t == MusicTrack::Menu && hasMenu) candidate = &musicMenu;
    if (t == MusicTrack::Game && hasGame) candidate = &musicGame;
    if (!candidate) return; // nothing to play (App may choose legacy fallback)

    nxtTrack = t; nxt = candidate;
    fadeDur = (crossfadeSec < 0.f) ? 0.f : crossfadeSec;
    fadeTime = fadeDur;

    SafeStart(nxt);
    SafeSet(nxt, 0.0f);
}

void Audio::Update(float dt) {
    if (cur) UpdateMusicStream(*cur);
    if (nxt) UpdateMusicStream(*nxt);

    if (nxt && fadeDur > 0.0f) {
        fadeTime -= dt;
        float a = (fadeDur <= 0.0f) ? 1.0f : (1.0f - fadeTime / fadeDur);
        if (a < 0.0f) a = 0.0f; if (a > 1.0f) a = 1.0f;

        float curVol = (1.0f - a) * volMaster * volMusic;
        float nxtVol = a * volMaster * volMusic;
        if (cur) SafeSet(cur, curVol);
        SafeSet(nxt, nxtVol);

        if (fadeTime <= 0.0f) {
            if (cur) SafeStop(cur);
            cur = nxt; curTrack = nxtTrack;
            nxt = nullptr; nxtTrack = MusicTrack::None;
            fadeDur = fadeTime = 0.0f;
        }
    }
}

void Audio::Play(SfxId id, float pitch, float volScale) {
    float v = volMaster * volSfx * volScale;
    if (id == SfxId::Click && hasClick) {
        SetSoundPitch(uiClick, pitch);
        SetSoundVolume(uiClick, v);
        PlaySound(uiClick);
    }
}

bool Audio::HasTrack(MusicTrack t) const {
    if (t == MusicTrack::Menu) return hasMenu;
    if (t == MusicTrack::Game) return hasGame;
    return false;
}

bool Audio::HasSfx(SfxId id) const {
    if (id == SfxId::Click) return hasClick;
    return false;
}

/* static */ void Audio::SafeStart(Music* m) { if (m && !IsMusicStreamPlaying(*m)) PlayMusicStream(*m); }
/* static */ void Audio::SafeStop(Music* m) { if (m && IsMusicStreamPlaying(*m)) StopMusicStream(*m); }
/* static */ void Audio::SafeSet(Music* m, float vol) { if (m) SetMusicVolume(*m, vol); }
