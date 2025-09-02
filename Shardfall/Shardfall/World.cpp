#include "World.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>

void World::Draw(const Camera2D& cam) const {
    DrawRectangleRec(bounds, Color{ 22,22,28,255 });

    const float step = 64.0f;
    Vector2 tl = GetScreenToWorld2D(Vector2{ 0,0 }, cam);
    Vector2 br = GetScreenToWorld2D(Vector2{ (float)GetScreenWidth(), (float)GetScreenHeight() }, cam);

    int x0 = (int)floorf((std::max(tl.x, bounds.x)) / step) - 1;
    int x1 = (int)ceilf((std::min(br.x, bounds.x + bounds.width)) / step) + 1;
    int y0 = (int)floorf((std::max(tl.y, bounds.y)) / step) - 1;
    int y1 = (int)ceilf((std::min(br.y, bounds.y + bounds.height)) / step) + 1;

    for (int gx = x0; gx <= x1; ++gx) {
        float x = gx * step;
        DrawLine((int)x, (int)(y0 * step), (int)x, (int)(y1 * step), Color{ 32,32,40,255 });
    }
    for (int gy = y0; gy <= y1; ++gy) {
        float y = gy * step;
        DrawLine((int)(x0 * step), (int)y, (int)(x1 * step), (int)y, Color{ 32,32,40,255 });
    }
    DrawRectangleLinesEx(bounds, 4, Color{ 60,60,80,255 });
}

void World::DrawMinimap(const Rectangle& panel, Vector2 playerPos, const Vector2* enemyPos, int enemyCount) const {
    DrawRectangleRec(panel, Color{ 18,18,22,220 });
    DrawRectangleLines((int)panel.x, (int)panel.y, (int)panel.width, (int)panel.height, Color{ 70,70,90,255 });

    auto mapX = [&](float wx) {
        return panel.x + (wx - bounds.x) / bounds.width * panel.width;
        };
    auto mapY = [&](float wy) {
        return panel.y + (wy - bounds.y) / bounds.height * panel.height;
        };

    float px = mapX(playerPos.x);
    float py = mapY(playerPos.y);
    DrawCircle((int)px, (int)py, 4.0f, Color{ 240,240,240,255 });

    for (int i = 0; i < enemyCount; ++i) {
        float ex = mapX(enemyPos[i].x);
        float ey = mapY(enemyPos[i].y);
        DrawCircle((int)ex, (int)ey, 3.0f, Color{ 120,220,120,255 });
    }
}
