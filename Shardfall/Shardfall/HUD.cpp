#include "HUD.h"
#include "Classes.h"
#include "Data.h"
#include <cstdio>

HUD::HUD(AppContext& app) : app_(app) {}

void HUD::Draw(const Player& /*player*/, float playerHp, float playerMaxHp,
    const EnemyManager& /*enemies*/, const ProjectileSystem& /*projectiles*/,
    const Spawner& spawner) const
{
    DrawRunWaveBoss(spawner);
    DrawPlayerHP(playerHp, playerMaxHp);
}

void HUD::DrawOverlay(const Player& /*player*/, float /*playerHp*/, float /*playerMaxHp*/,
    const EnemyManager& enemies, const ProjectileSystem& projectiles,
    const Spawner& spawner) const
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{ 0,0,0,160 });

    int x = 60, y = 80, lh = 28;
    DrawText("Paused", x, y, 48, RAYWHITE); y += 56;

    char buf[256];
    std::snprintf(buf, sizeof(buf), "FPS: %d", GetFPS());
    DrawText(buf, x, y, 24, RAYWHITE); y += lh;

    std::snprintf(buf, sizeof(buf), "Enemies: %d", (int)enemies.All().size());
    DrawText(buf, x, y, 24, RAYWHITE); y += lh;

    std::snprintf(buf, sizeof(buf), "Projectiles: %d", (int)projectiles.Count());
    DrawText(buf, x, y, 24, RAYWHITE); y += lh;

    std::snprintf(buf, sizeof(buf), "Class: %s", ToString(app_.profile.cls));
    DrawText(buf, x, y, 24, RAYWHITE); y += lh;

    char t[8]; Spawner::FormatMMSS(t, spawner.GetRunTimeSec());
    char b[8]; Spawner::FormatMMSS(b, spawner.GetBossTimeSec());
    std::snprintf(buf, sizeof(buf), "Run: %s   |   Wave: %d   |   Boss at %s%s",
        t, spawner.GetWave(), b, spawner.BossSpawned() ? " (spawned)" : "");
    DrawText(buf, x, y, 24, RAYWHITE); y += lh;

    y += 12;
    DrawText("Hotkeys:", x, y, 24, RAYWHITE); y += lh;
    DrawText(" - F1 / TAB: Toggle Pause/Debug", x, y, 22, LIGHTGRAY); y += lh;
    DrawText(" - LMB / Space: Primary Attack", x, y, 22, LIGHTGRAY); y += lh;
    DrawText(" - 1..6: Cast equipped abilities", x, y, 22, LIGHTGRAY); y += lh;
    DrawText(" - Wheel: Zoom    ESC: Menu", x, y, 22, LIGHTGRAY); y += lh;
}

void HUD::DrawPlayerHP(float hp, float maxHp) const {
    const int x = 20, y = 16;
    const int w = 260, h = 20;

    float pct = (maxHp > 0.0f) ? (hp / maxHp) : 0.0f;
    if (pct < 0) pct = 0; if (pct > 1) pct = 1;

    DrawText("HP", x, y - 22, 26, RAYWHITE);
    DrawRectangle(x, y, w, h, Color{ 30,30,36,230 });
    DrawRectangle(x, y, (int)(w * pct), h, Color{ 220,70,70,230 });
    DrawRectangleLines(x, y, w, h, Color{ 230,230,230,220 });

    char txt[32]; std::snprintf(txt, sizeof(txt), "%.0f / %.0f", hp, maxHp);
    int fs = 20; int tw = MeasureText(txt, fs);
    DrawText(txt, x + (w - tw) / 2, y - 1, fs, RAYWHITE);
}

void HUD::DrawRunWaveBoss(const Spawner& spawner) const {
    char timeTxt[8];  Spawner::FormatMMSS(timeTxt, spawner.GetRunTimeSec());
    char bossTxt[8];  Spawner::FormatMMSS(bossTxt, spawner.GetBossTimeSec());

    const int fs = 28;
    char line[128];
    std::snprintf(line, sizeof(line), "Time %s   |   Wave %d   |   Boss at %s%s",
        timeTxt, spawner.GetWave(), bossTxt, spawner.BossSpawned() ? " (spawned)" : "");

    int tw = MeasureText(line, fs);
    DrawText(line, (GetScreenWidth() - tw) / 2, 12, fs, RAYWHITE);
}
