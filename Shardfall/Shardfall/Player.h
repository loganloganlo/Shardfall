#pragma once
#include "raylib.h"

struct Player {
    Vector2 pos{ 0,0 };
    float   speed = 320.0f;
    float   angleDeg = 0.0f;
    float   half = 24.0f;

    void EnsureInit(const Rectangle& worldBounds);
    void UpdateInput(float dt, bool inputEnabled, const Rectangle& worldBounds);
    void Face(Vector2 worldMouse);
};
