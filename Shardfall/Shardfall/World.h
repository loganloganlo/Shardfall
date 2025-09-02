#pragma once
#include "raylib.h"

class World {
public:
    Rectangle bounds{ -1000.0f, -600.0f, 2000.0f, 1200.0f };

    void Draw(const Camera2D& cam) const;
    void DrawMinimap(const Rectangle& panel, Vector2 playerPos, const Vector2* enemyPos, int enemyCount) const;
};
