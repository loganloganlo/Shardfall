#include "CameraController.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>

namespace {
    constexpr float ZOOM_MIN = 0.75f;
    constexpr float ZOOM_MAX = 1.75f;
    constexpr float ZOOM_STEP = 0.10f;
    constexpr float FOLLOW_LERP = 10.0f;
}

void CameraController::Init(Vector2 playerPos) {
    cam.target = playerPos;
    cam.offset = { (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.5f };
    cam.rotation = 0.0f;
    if (cam.zoom == 0.0f) cam.zoom = 1.0f;
}

void CameraController::Update(float dt, Vector2 playerPos, float /*zoomMin*/, float /*zoomMax*/) {
    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f) {
        cam.zoom += wheel * ZOOM_STEP;
        if (cam.zoom < ZOOM_MIN) cam.zoom = ZOOM_MIN;
        if (cam.zoom > ZOOM_MAX) cam.zoom = ZOOM_MAX;
    }
    // follow
    cam.target.x += (playerPos.x - cam.target.x) * std::min(1.0f, FOLLOW_LERP * dt);
    cam.target.y += (playerPos.y - cam.target.y) * std::min(1.0f, FOLLOW_LERP * dt);

    // base offset (screen center)
    Vector2 ofs{ (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.5f };

    // shake
    if (shakeAmp_ > 0.01f) {
        shakeTime_ += dt;
        float s = std::sinf(shakeTime_ * 40.0f) * shakeAmp_;
        ofs.x += s;
        ofs.y += s * -0.6f;
        // decay
        shakeAmp_ *= std::exp(-dt * 6.0f);
    }
    else {
        shakeAmp_ = 0.0f;
    }
    cam.offset = ofs;
}

Vector2 CameraController::WorldMouse() const {
    return GetScreenToWorld2D(GetMousePosition(), cam);
}

void CameraController::AddShake(float amount) {
    // accumulate up to a cap
    float cap = 15.0f;
    shakeAmp_ += amount;
    if (shakeAmp_ > cap) shakeAmp_ = cap;
}
