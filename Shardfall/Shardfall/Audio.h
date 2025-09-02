#pragma once
#include "raylib.h"
#include <string>

enum class MusicTrack { None, Menu, Game };
enum class SfxId { Click };

class Audio {
public:
    void Init(const std::string& assetDir = "assets/");
    void Update(float dt);

    // Cross-fade to a track if present
    void SetTrack(MusicTrack t, float crossfadeSec = 0.5f);

    // Volume routing (0..1 each)
    void SetVolumes(float master, float music, float sfx);

    // SFX
    void Play(SfxId id, float pitch = 1.0f, float volScale = 1.0f);

    // Introspection (so App can choose legacy fallback if needed)
    bool HasTrack(MusicTrack t) const;
    bool HasSfx(SfxId id) const;

private:
    // Music
    Music musicMenu{}, musicGame{};
    bool hasMenu = false, hasGame = false;
    Music* cur = nullptr;
    Music* nxt = nullptr;
    MusicTrack curTrack = MusicTrack::None;
    MusicTrack nxtTrack = MusicTrack::None;
    float fadeTime = 0.0f, fadeDur = 0.0f;

    // Volumes
    float volMaster = 1.0f, volMusic = 0.7f, volSfx = 1.0f;

    // SFX
    Sound uiClick{}; bool hasClick = false;

    // Helpers
    static void SafeStart(Music* m);
    static void SafeStop(Music* m);
    static void SafeSet(Music* m, float vol);

    // File search
    static std::string FindAsset(const char* filename);
    static std::string FirstExisting(const char* const* names, int count);
};
