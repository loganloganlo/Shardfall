// Shardfall.cpp  — main entry
#include "raylib.h"
#include <memory>

// Yours
#include "Config.h"
#include "App.h"
#include "Scene.h"
#include "MenuScene.h"
#include "SettingsScene.h"
#include "LoadoutScene.h"
#include "GameScene.h"

// --------- helpers ---------
static std::unique_ptr<Scene> MakeScene(AppContext& app, SceneRequest req) {
    switch (req) {
    case SceneRequest::Menu:     return std::make_unique<MenuScene>(app);
    case SceneRequest::Settings: return std::make_unique<SettingsScene>(app);
    case SceneRequest::Loadout:  return std::make_unique<LoadoutScene>(app);
    case SceneRequest::Game:     return std::make_unique<GameScene>(app);
    default:                     return nullptr;
    }
}

int main() {
    // App context holds settings/profile/audio state
    AppContext app;

    // 1) Load settings FIRST (from AppData). No window calls here.
    app.LoadSettings();

    // 2) Create the window using those settings
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    if (app.fullscreen) SetConfigFlags(FLAG_FULLSCREEN_MODE);

    const int monitor = 0; // primary monitor
    const int initW = app.fullscreen ? GetMonitorWidth(monitor) : app.windowW;
    const int initH = app.fullscreen ? GetMonitorHeight(monitor) : app.windowH;

    InitWindow(initW, initH, CFG::WINDOW_TITLE);
    SetTargetFPS(CFG::TARGET_FPS);

    // 3) Profile + Audio
    app.LoadProfile();        // loads/creates profile from stable path
    SetMasterVolume(app.masterVolume);  // apply saved volume to the device
    app.InitAudio();          // starts menu music if present, hooks SFX/music manager

    // 4) Start at menu
    std::unique_ptr<Scene> scene = std::make_unique<MenuScene>(app);

    // 5) Main loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Scene update/draw
        scene->Update(dt);

        // Keep audio cross-fades/SFX alive
        app.UpdateAudio(dt);

        BeginDrawing();
        scene->Draw();
        EndDrawing();

        // Scene transitions
        if (SceneRequest req = scene->ConsumeRequest(); req != SceneRequest::None) {
            if (req == SceneRequest::Quit) break;

            if (auto next = MakeScene(app, req)) {
                scene = std::move(next);
            }
            else {
                scene = std::make_unique<MenuScene>(app);
            }
        }
    }

    // 6) Persist any changes
    app.SaveProfile();
    app.SaveSettings();

    // 7) Clean shutdown
    CloseAudioDevice();   // safe even if already closed inside audio code
    CloseWindow();
    return 0;
}
