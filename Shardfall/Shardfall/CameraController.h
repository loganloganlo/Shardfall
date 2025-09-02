#pragma once
#include "raylib.h"

class CameraController {
public:
    Camera2D cam{};

    void Init(Vector2 playerPos);
    void Update(float dt, Vector2 playerPos, float zoomMin = 0.75f, float zoomMax = 1.75f);
    Vector2 WorldMouse() const;

    // Hit feedback
    void AddShake(float amount); // small values 1..6 feel good

private:
    float shakeAmp_ = 0.0f;
    float shakeTime_ = 0.0f;
};
