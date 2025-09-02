#include "Player.h"
#include <cmath>

void Player::EnsureInit(const Rectangle& worldBounds) {
    if (pos.x == 0 && pos.y == 0) {
        pos = { worldBounds.x + worldBounds.width * 0.5f,
                worldBounds.y + worldBounds.height * 0.5f };
    }
}

void Player::UpdateInput(float dt, bool inputEnabled, const Rectangle& worldBounds) {
    if (!inputEnabled) return;

    float x = 0, y = 0;
    if (IsKeyDown(KEY_A)) x -= 1.0f;
    if (IsKeyDown(KEY_D)) x += 1.0f;
    if (IsKeyDown(KEY_W)) y -= 1.0f;
    if (IsKeyDown(KEY_S)) y += 1.0f;

    float len = std::sqrt(x * x + y * y);
    if (len > 0.0001f) { x /= len; y /= len; }

    pos.x += x * speed * dt;
    pos.y += y * speed * dt;

    float minX = worldBounds.x + half;
    float minY = worldBounds.y + half;
    float maxX = worldBounds.x + worldBounds.width - half;
    float maxY = worldBounds.y + worldBounds.height - half;
    if (pos.x < minX) pos.x = minX;
    if (pos.y < minY) pos.y = minY;
    if (pos.x > maxX) pos.x = maxX;
    if (pos.y > maxY) pos.y = maxY;
}

void Player::Face(Vector2 wm) {
    angleDeg = std::atan2f(wm.y - pos.y, wm.x - pos.x) * RAD2DEG;
}
