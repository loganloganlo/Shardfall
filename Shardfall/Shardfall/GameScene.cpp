#include "GameScene.h"
#include "App.h"
#include "Classes.h"
#include "Data.h"
#include "raylib.h"
#include <cmath>
#include <vector>

GameScene::GameScene(AppContext& app)
    : Scene(app)
    , world_()
    , camera_()
    , player_()
    , enemies_()
    , projectiles_()
    , fx_()
    , abilities_(app, player_, enemies_, world_, camera_, projectiles_, fx_)
    , spawner_()
    , hud_(app)
{
    app_.EnsureGameMusic(); // stop menu music, start game music

    player_.EnsureInit(world_.bounds);
    player_.speed = GetClassSpec(app_.profile.cls).baseSpeed;

    camera_.Init(player_.pos);
    abilities_.BuildFromProfile();

    spawner_.Init(180.0f);

    playerMaxHp_ = 100.0f;
    playerHp_ = playerMaxHp_;
    hurtCd_ = 0.0f;
    hurtFlash_ = 0.0f;
}

void GameScene::Update(float dt) {
    if (IsKeyPressed(KEY_F1) || IsKeyPressed(KEY_TAB)) {
        paused_ = !paused_;
        showOverlay_ = paused_;
        app_.PlayClick(); // UI sound is OK here
    }

    if (!paused_) {
        player_.Face(camera_.WorldMouse());
        const bool inputEnabled = !abilities_.InputBlocked();
        player_.UpdateInput(dt, inputEnabled, world_.bounds);

        enemies_.Update(dt, player_.pos, world_.bounds);
        projectiles_.Update(dt, enemies_, fx_, camera_);
        fx_.Update(dt);
        abilities_.Update(dt);
        spawner_.Update(dt, enemies_, world_.bounds, player_.pos, /*paused*/false);
        camera_.Update(dt, player_.pos);

        abilities_.HandleInput();

        if (IsKeyPressed(KEY_ESCAPE)) {
            app_.PlayClick();
            Request(SceneRequest::Menu);
        }

        if (hurtCd_ > 0.0f) { hurtCd_ -= dt; if (hurtCd_ < 0) hurtCd_ = 0; }
        if (hurtCd_ <= 0.0f) {
            for (const auto& e : enemies_.All()) {
                float dx = e.pos.x - player_.pos.x;
                float dy = e.pos.y - player_.pos.y;
                float rr = (e.radius + player_.half);
                if (dx * dx + dy * dy <= rr * rr) {
                    float dmg = (e.radius > 30.0f) ? 20.0f : 10.0f;
                    playerHp_ -= dmg;
                    if (playerHp_ < 0.0f) playerHp_ = 0.0f;
                    hurtCd_ = 0.60f;
                    hurtFlash_ = 0.22f;
                    break;
                }
            }
        }

        if (playerHp_ <= 0.0f) {
            app_.PlayClick();
            Request(SceneRequest::Menu);
        }

    }
    else {
        camera_.Update(0.0f, player_.pos);
        spawner_.Update(0.0f, enemies_, world_.bounds, player_.pos, /*paused*/true);
        if (IsKeyPressed(KEY_ESCAPE)) {
            app_.PlayClick();
            Request(SceneRequest::Menu);
        }
    }

    if (hurtFlash_ > 0.0f) {
        hurtFlash_ -= dt;
        if (hurtFlash_ < 0.0f) hurtFlash_ = 0.0f;
    }
}

void GameScene::Draw() {
    ClearBackground(Color{ 8,8,10,255 });

    BeginMode2D(camera_.cam);
    {
        world_.Draw(camera_.cam);
        enemies_.Draw();

        const ClassSpec& cls = GetClassSpec(app_.profile.cls);

        if (cls.shape == ClassSpec::Square) {
            const float size = player_.half * 2.0f;
            DrawRectanglePro(
                Rectangle{ player_.pos.x, player_.pos.y, size, size },
                Vector2{ size * 0.5f, size * 0.5f },
                player_.angleDeg, cls.color);
            Vector2 tip{
                player_.pos.x + std::cos(player_.angleDeg * DEG2RAD) * player_.half,
                player_.pos.y + std::sin(player_.angleDeg * DEG2RAD) * player_.half
            };
            DrawLineEx(player_.pos, tip, 3.0f, Color{ 255,255,255,220 });

        }
        else if (cls.shape == ClassSpec::Circle) {
            const float r = player_.half;
            DrawCircleV(player_.pos, r, cls.color);
            Vector2 tip{
                player_.pos.x + std::cos(player_.angleDeg * DEG2RAD) * r,
                player_.pos.y + std::sin(player_.angleDeg * DEG2RAD) * r
            };
            DrawLineEx(player_.pos, tip, 3.0f, Color{ 255,255,255,220 });

        }
        else { // Triangle (Rogue)
            DrawPoly(player_.pos, 3, player_.half, player_.angleDeg, cls.color);
            Vector2 tip{
                player_.pos.x + std::cos(player_.angleDeg * DEG2RAD) * player_.half,
                player_.pos.y + std::sin(player_.angleDeg * DEG2RAD) * player_.half
            };
            DrawLineEx(player_.pos, tip, 3.0f, Color{ 255,255,255,220 });
        }

        projectiles_.DrawWorld();
        fx_.DrawWorld();
        abilities_.DrawWorldOverlay();
    }
    EndMode2D();

    if (hurtFlash_ > 0.0f) {
        float t = hurtFlash_ / 0.22f;
        unsigned char a = (unsigned char)(t * 140);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{ 255, 40, 40, a });
    }

    hud_.Draw(player_, playerHp_, playerMaxHp_, enemies_, projectiles_, spawner_);

    DrawText("F1/TAB: Pause/Debug  |  LMB/Space: Primary  |  1-6: Abilities  |  Wheel: Zoom  |  ESC: Menu",
        40, GetScreenHeight() - 36, 20, LIGHTGRAY);

    abilities_.DrawUI();

    Rectangle panel{ (float)GetScreenWidth() - 200.0f - 16.0f, 16.0f, 200.0f, 120.0f };
    const auto& es = enemies_.All();
    std::vector<Vector2> dots; dots.reserve(es.size());
    for (const auto& e : es) dots.push_back(e.pos);
    world_.DrawMinimap(panel, player_.pos, dots.data(), (int)dots.size());

    if (showOverlay_) {
        hud_.DrawOverlay(player_, playerHp_, playerMaxHp_, enemies_, projectiles_, spawner_);
    }
}
