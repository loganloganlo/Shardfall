#include "LoadoutScene.h"
#include "raylib.h"
#include <algorithm>

// ---- ctor ----
LoadoutScene::LoadoutScene(AppContext& app)
    : Scene(app)
{
    classes_ = { ClassId::Warrior, ClassId::Mage, ClassId::Rogue };

    // start from current profile class
    auto it = std::find(classes_.begin(), classes_.end(), app_.profile.cls);
    classIdx_ = (it != classes_.end()) ? int(it - classes_.begin()) : 0;

    RebuildForClass();
    EnsureDefaultsIfEmpty();
}

// ---- helpers ----
void LoadoutScene::RebuildForClass() {
    app_.profile.cls = classes_[classIdx_];

    // full ability list for this class
    abilities_ = GetAbilitiesForClass(app_.profile.cls);

    // clamp cursor
    if (abilityCursor_ >= (int)abilities_.size()) abilityCursor_ = (int)abilities_.size() - 1;
    if (abilityCursor_ < 0) abilityCursor_ = 0;
}

void LoadoutScene::EnsureDefaultsIfEmpty() {
    // If nothing equipped yet, use class defaults (up to 6)
    if (app_.profile.equipped.empty()) {
        const ClassSpec& spec = GetClassSpec(app_.profile.cls);
        app_.profile.equipped = spec.defaultEquipped;
        if (app_.profile.equipped.size() > 6) app_.profile.equipped.resize(6);
    }
}

// ---- update ----
void LoadoutScene::Update(float /*dt*/) {
    // class change
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        classIdx_ = (classIdx_ + 1) % (int)classes_.size();
        app_.PlayClick();
        RebuildForClass();
        // wipe equips if changing class? keep current behavior: keep but filter invalid
        // Remove equips not belonging to new class
        {
            std::vector<AbilityId> allowed = GetAbilitiesForClass(app_.profile.cls);
            app_.profile.equipped.erase(
                std::remove_if(app_.profile.equipped.begin(), app_.profile.equipped.end(),
                    [&](AbilityId a) { return std::find(allowed.begin(), allowed.end(), a) == allowed.end(); }),
                app_.profile.equipped.end());
            EnsureDefaultsIfEmpty();
        }
    }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        classIdx_ = (classIdx_ - 1 + (int)classes_.size()) % (int)classes_.size();
        app_.PlayClick();
        RebuildForClass();
        std::vector<AbilityId> allowed = GetAbilitiesForClass(app_.profile.cls);
        app_.profile.equipped.erase(
            std::remove_if(app_.profile.equipped.begin(), app_.profile.equipped.end(),
                [&](AbilityId a) { return std::find(allowed.begin(), allowed.end(), a) == allowed.end(); }),
            app_.profile.equipped.end());
        EnsureDefaultsIfEmpty();
    }

    // navigate ability list
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        abilityCursor_ = (abilityCursor_ + 1) % (int)abilities_.size();
        app_.PlayClick(0.7f);
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        abilityCursor_ = (abilityCursor_ - 1 + (int)abilities_.size()) % (int)abilities_.size();
        app_.PlayClick(0.7f);
    }

    // toggle equip (space/enter)
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        AbilityId pick = abilities_[abilityCursor_];
        auto& eq = app_.profile.equipped;
        auto it = std::find(eq.begin(), eq.end(), pick);
        if (it == eq.end()) {
            if ((int)eq.size() < 6) {
                eq.push_back(pick);
                app_.PlayClick(0.9f);
            }
            else {
                // swap in the selected slot? Here: replace the last
                eq.back() = pick;
                app_.PlayClick(0.6f);
            }
        }
        else {
            // unequip
            eq.erase(it);
            app_.PlayClick(0.6f);
        }
    }

    // go play
    if (IsKeyPressed(KEY_P)) {
        app_.PlayClick();
        Request(SceneRequest::Game);
    }

    // back to menu
    if (IsKeyPressed(KEY_ESCAPE)) {
        app_.PlayClick();
        Request(SceneRequest::Menu);
    }
}

// ---- draw ----
void LoadoutScene::Draw() {
    ClearBackground(Color{ 12, 12, 16, 255 });

    const int sw = GetScreenWidth();
    const int sh = GetScreenHeight();

    // Title / class header
    const char* className = ToString(classes_[classIdx_]);
    int titleFs = 42;
    int tw = MeasureText(className, titleFs);
    DrawText(className, (sw - tw) / 2, 30, titleFs, RAYWHITE);

    // hint: change class
    DrawText("A/D or \x1B[<-\x1B[-> to change class    |    SPACE/ENTER to equip/unequip ability",
        40, 90, 20, LIGHTGRAY);
    DrawText("P: Play     ESC: Menu", 40, 115, 20, LIGHTGRAY);

    // Equipped panel (right)
    int panelW = 360;
    int panelX = sw - panelW - 40;
    int panelY = 160;
    int panelH = sh - panelY - 40;
    DrawRectangle(panelX, panelY, panelW, panelH, Color{ 20, 20, 26, 220 });
    DrawRectangleLines(panelX, panelY, panelW, panelH, Color{ 90, 90, 110, 255 });
    DrawText("Equipped (max 6)", panelX + 12, panelY + 10, 24, RAYWHITE);

    // list equipped
    const int lh = 26;
    int y = panelY + 44;
    for (int i = 0; i < 6; ++i) {
        const char* label = "(empty)";
        if (i < (int)app_.profile.equipped.size()) {
            label = ToString(app_.profile.equipped[i]);
        }
        DrawText(TextFormat("%d) %s", i + 1, label), panelX + 16, y, 22, LIGHTGRAY);
        y += lh;
    }

    // Abilities list (left)
    int listX = 40;
    int listY = 160;
    DrawText("Abilities:", listX, listY - 28, 26, RAYWHITE);

    for (int i = 0; i < (int)abilities_.size(); ++i) {
        bool equipped = std::find(app_.profile.equipped.begin(),
            app_.profile.equipped.end(),
            abilities_[i]) != app_.profile.equipped.end();
        Color col = equipped ? Color{ 120, 220, 140, 255 } : LIGHTGRAY;
        if (i == abilityCursor_) {
            DrawRectangle(listX - 10, listY + i * lh - 4, 460, lh, Color{ 60, 60, 90, 180 });
        }
        DrawText(TextFormat("%s%s",
            equipped ? "[E] " : "",
            ToString(abilities_[i])),
            listX, listY + i * lh, 22, col);
    }
}
