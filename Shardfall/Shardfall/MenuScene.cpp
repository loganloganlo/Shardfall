#include "MenuScene.h"
#include "raylib.h"

// Find asset helper
static std::string FindAssetPath(const char* filename) {
    std::string c1 = std::string(filename);
    std::string c2 = std::string("assets/") + filename;
    std::string c3 = std::string("../assets/") + filename;
    std::string c4 = std::string("../../assets/") + filename;

    if (FileExists(c1.c_str())) return c1;
    if (FileExists(c2.c_str())) return c2;
    if (FileExists(c3.c_str())) return c3;
    if (FileExists(c4.c_str())) return c4;
    return c2;
}

MenuScene::MenuScene(AppContext& app) : Scene(app) {
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    float bw = w * 0.28f;
    float bh = 56.0f;
    float x = (w - bw) * 0.5f;
    float y1 = h * 0.55f;
    float y2 = y1 + bh + 16.0f;

    buttons_.emplace_back(Rectangle{ x, y1, bw, bh }, "Play", 28);
    buttons_.emplace_back(Rectangle{ x, y2, bw, bh }, "Settings", 28);

    // Background
    bgPath_ = FindAssetPath("Shardfall_Background.png");
    if (FileExists(bgPath_.c_str())) {
        bg_ = LoadTexture(bgPath_.c_str());
        bgLoaded_ = (bg_.id != 0);
    }
}

MenuScene::~MenuScene() {
    if (bgLoaded_) UnloadTexture(bg_);
}

void MenuScene::Update(float) {
    app_.EnsureMenuMusic();

    Vector2 m = GetMousePosition();
    for (size_t i = 0; i < buttons_.size(); ++i)
        if (CheckCollisionPointRec(m, buttons_[i].rect)) selected_ = (int)i;

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) selected_ = (selected_ + 1) % (int)buttons_.size();
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) selected_ = (selected_ - 1 + (int)buttons_.size()) % (int)buttons_.size();

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || IsKeyPressed(KEY_SPACE)) {
        app_.PlayClick();
        if (selected_ == 0) Request(SceneRequest::Loadout);
        if (selected_ == 1) Request(SceneRequest::Settings);
    }
    if (buttons_[0].Update()) { app_.PlayClick(); Request(SceneRequest::Loadout); }
    if (buttons_[1].Update()) { app_.PlayClick(); Request(SceneRequest::Settings); }

    if (IsKeyPressed(KEY_ESCAPE)) Request(SceneRequest::Quit);
}

void MenuScene::Draw() {
    if (bgLoaded_) {
        const float sw = (float)bg_.width, sh = (float)bg_.height;
        const float ww = (float)GetScreenWidth(), wh = (float)GetScreenHeight();
        float scale = (ww / sw > wh / sh) ? (ww / sw) : (wh / sh);
        Rectangle src{ 0, 0, sw, sh };
        Rectangle dst{ (ww - sw * scale) * 0.5f, (wh - sh * scale) * 0.5f, sw * scale, sh * scale };
        DrawTexturePro(bg_, src, dst, Vector2{ 0,0 }, 0.0f, WHITE);
        DrawRectangle(0, 0, (int)ww, (int)wh, Fade(BLACK, 0.35f));
    }
    else {
        ClearBackground(Color{ 18,18,24,255 });
    }

    const char* title = "Shardfall";
    int titleSize = 64;
    int tw = MeasureText(title, titleSize);
    DrawText(title, (GetScreenWidth() - tw) / 2, (int)(GetScreenHeight() * 0.18f), titleSize, RAYWHITE);

    const char* sub = "A top-down roguelike";
    int subSize = 20;
    int sw = MeasureText(sub, subSize);
    DrawText(sub, (GetScreenWidth() - sw) / 2, (int)(GetScreenHeight() * 0.18f) + 76, subSize, LIGHTGRAY);

    for (size_t i = 0; i < buttons_.size(); ++i)
        buttons_[i].Draw((int)i == selected_);

    DrawText("v0.0.1", 16, GetScreenHeight() - 28, 18, GRAY);
}
