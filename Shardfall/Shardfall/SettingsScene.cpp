#include "SettingsScene.h"
#include "Config.h"
#include <algorithm>
#include <cstdio>

static inline float clamp01(float v) { return v < 0.f ? 0.f : (v > 1.f ? 1.f : v); }

SettingsScene::SettingsScene(AppContext& app) : Scene(app) {
    // Common 16:9 / 16:10 choices (add more if you like)
    options_ = {
        {1280, 720}, {1366, 768}, {1600, 900}, {1920,1080}, {2560,1440}, {3840,2160},
        {1280, 800}, {1440, 900}, {1680,1050}, {1920,1200}
    };

    // Find current res in list (or closest)
    int cw = GetScreenWidth(), ch = GetScreenHeight();
    int best = 0; int bestDelta = 1e9;
    for (int i = 0; i < (int)options_.size(); ++i) {
        int d = std::abs(options_[i].w - cw) + std::abs(options_[i].h - ch);
        if (d < bestDelta) { bestDelta = d; best = i; }
    }
    resIndex_ = best;
}

void SettingsScene::ApplyCurrentResolution() {
    if (app_.fullscreen) return; // don't resize while fullscreen
    const auto& r = options_[resIndex_];
    SetWindowSize(r.w, r.h);
    app_.windowW = r.w;
    app_.windowH = r.h;
}

bool SettingsScene::ButtonClicked(const Rectangle& r) const {
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(GetMousePosition(), r);
}

void SettingsScene::DrawButton(const Rectangle& r, const char* label, bool focused) const {
    Color fill = focused ? Color{ 55,55,60,255 } : Color{ 40,40,45,255 };
    DrawRectangleRec(r, fill);
    DrawRectangleLines((int)r.x, (int)r.y, (int)r.width, (int)r.height, RAYWHITE);
    int fs = 28;
    int tw = MeasureText(label, fs);
    DrawText(label, (int)(r.x + (r.width - tw) / 2), (int)(r.y + (r.height - fs) / 2), fs, RAYWHITE);
}

void SettingsScene::Update(float dt) {
    // Volume slider drag
    Vector2 m = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(m, volBar_))
        draggingVol_ = true;
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) draggingVol_ = false;
    if (draggingVol_) {
        float t = clamp01((m.x - volBar_.x) / volBar_.width);
        app_.masterVolume = t;
        SetMasterVolume(app_.masterVolume);
    }

    // Fullscreen toggle
    if (ButtonClicked(btnFullscreen_)) {
        app_.PlayClick();
        app_.fullscreen = !app_.fullscreen;
        ToggleFullscreen();
    }

    // Resolution step (only in windowed)
    if (!app_.fullscreen) {
        if (ButtonClicked(btnResLeft_)) { app_.PlayClick(0.95f); resIndex_ = (resIndex_ + (int)options_.size() - 1) % (int)options_.size(); ApplyCurrentResolution(); }
        if (ButtonClicked(btnResRight_)) { app_.PlayClick(1.05f); resIndex_ = (resIndex_ + 1) % (int)options_.size(); ApplyCurrentResolution(); }
    }

    // Back
    if (ButtonClicked(btnBack_) || IsKeyPressed(KEY_ESCAPE)) {
        app_.PlayClick();
        Request(SceneRequest::Menu);
    }

    // SAVE — writes settings.cfg now (NEW)
    if (ButtonClicked(btnSave_)) {
        app_.PlayClick(1.1f);
        app_.SaveSettings();
        savedToast_ = 1.5f; // show toast
    }

    if (savedToast_ > 0.0f) savedToast_ -= dt;
}

void SettingsScene::Draw() {
    ClearBackground(Color{ 10,10,12,255 });

    DrawText("Settings", 40, 40, 64, RAYWHITE);

    // Volume
    DrawText("Master Volume", 40, 130, 28, RAYWHITE);
    DrawRectangleRec(volBar_, Color{ 80,80,85,255 });
    float knobX = volBar_.x + volBar_.width * clamp01(app_.masterVolume) - 8.0f;
    DrawRectangle((int)knobX, (int)volBar_.y - 6, 16, (int)volBar_.height + 12, RAYWHITE);

    char volTxt[16]; std::snprintf(volTxt, sizeof(volTxt), "%.0f%%", app_.masterVolume * 100.f);
    DrawText(volTxt, (int)(volBar_.x + volBar_.width + 20), (int)volBar_.y - 10, 28, RAYWHITE);

    // Fullscreen
    DrawButton(btnFullscreen_, "Fullscreen", CheckCollisionPointRec(GetMousePosition(), btnFullscreen_));
    DrawText(app_.fullscreen ? "ON" : "OFF", (int)(btnFullscreen_.x + btnFullscreen_.width + 20), (int)btnFullscreen_.y + 10, 28, RAYWHITE);

    // Resolution
    DrawText("Resolution", 40, 320, 28, RAYWHITE);
    DrawButton(btnResLeft_, "<", false);
    DrawButton(btnResRight_, ">", false);

    const auto& r = options_[resIndex_];
    char resTxt[64]; std::snprintf(resTxt, sizeof(resTxt), "%d x %d", r.w, r.h);
    DrawText(resTxt, (int)(btnResLeft_.x + 60), (int)btnResLeft_.y + 12, 32, RAYWHITE);

    // Bottom buttons
    DrawButton(btnBack_, "Back", CheckCollisionPointRec(GetMousePosition(), btnBack_));
    DrawButton(btnSave_, "Save", CheckCollisionPointRec(GetMousePosition(), btnSave_)); // NEW

    if (savedToast_ > 0.0f) {
        const char* t = "Settings saved";
        int fs = 26; int tw = MeasureText(t, fs);
        DrawText(t, GetScreenWidth() - tw - 24, GetScreenHeight() - fs - 20, fs, Color{ 180,255,180,255 });
    }
}
