#pragma once
#include "Scene.h"
#include "App.h"
#include "raylib.h"
#include <vector>

class SettingsScene : public Scene {
public:
    explicit SettingsScene(AppContext& app);

    void Update(float dt) override;
    void Draw() override;

private:
    // UI state
    Rectangle volBar_{ 40, 190, 330, 18 };
    bool draggingVol_ = false;

    Rectangle btnFullscreen_{ 40, 260, 210, 48 };
    Rectangle btnResLeft_{ 40, 350, 48, 48 };
    Rectangle btnResRight_{ 310, 350, 48, 48 };
    Rectangle btnBack_{ 40, 640, 180, 56 };
    Rectangle btnSave_{ 240, 640, 180, 56 }; // NEW

    // Resolution choices
    struct Res { int w, h; };
    std::vector<Res> options_;
    int resIndex_ = 0;

    // Toast after saving
    float savedToast_ = 0.0f;

    // helpers
    void ApplyCurrentResolution();
    void DrawButton(const Rectangle& r, const char* label, bool focused = false) const;
    bool ButtonClicked(const Rectangle& r) const;
};
