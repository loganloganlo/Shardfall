#include "Enemy.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>

void EnemyManager::Clear() {
    enemies_.clear();
}

void EnemyManager::Damage(Enemy& e, float amount) {
    e.hp -= amount;
    if (e.hp <= 0.0f) e.alive = false;
}

void EnemyManager::SpawnBasic(Vector2 pos) {
    Enemy e;
    e.pos = pos;
    e.radius = 16.0f;
    e.hp = 60.0f;
    e.maxHp = e.hp;
    e.speed = 120.0f;
    e.color = Color{ 200, 70, 80, 255 };
    enemies_.push_back(e);
}

void EnemyManager::SpawnBoss(Vector2 pos) {
    Enemy e;
    e.pos = pos;
    e.radius = 38.0f;
    e.hp = 900.0f;
    e.maxHp = e.hp;
    e.speed = 85.0f;
    e.color = Color{ 220, 140, 60, 255 };
    enemies_.push_back(e);
}

void EnemyManager::Update(float dt, Vector2 playerPos, const Rectangle& bounds) {
    for (auto& e : enemies_) {
        if (!e.alive) continue;

        if (e.stun > 0.0f) { e.stun -= dt; if (e.stun < 0) e.stun = 0; }

        if (e.stun <= 0.0f) {
            Vector2 d{ playerPos.x - e.pos.x, playerPos.y - e.pos.y };
            float len = std::sqrt(d.x * d.x + d.y * d.y);
            if (len > 0.001f) { d.x /= len; d.y /= len; }
            e.vel = { d.x * e.speed, d.y * e.speed };
            e.pos.x += e.vel.x * dt;
            e.pos.y += e.vel.y * dt;
        }

        float minX = bounds.x + e.radius;
        float minY = bounds.y + e.radius;
        float maxX = bounds.x + bounds.width - e.radius;
        float maxY = bounds.y + bounds.height - e.radius;
        if (e.pos.x < minX) e.pos.x = minX;
        if (e.pos.y < minY) e.pos.y = minY;
        if (e.pos.x > maxX) e.pos.x = maxX;
        if (e.pos.y > maxY) e.pos.y = maxY;
    }

    enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(),
        [](const Enemy& e) { return !e.alive || e.hp <= 0.0f; }), enemies_.end());
}

void EnemyManager::Draw() const {
    for (const auto& e : enemies_) {
        if (!e.alive) continue;

        DrawCircleV(e.pos, e.radius, e.color);
        DrawCircleLines((int)e.pos.x, (int)e.pos.y, e.radius, Color{ 255,255,255,70 });

        // health bar
        float pct = (e.maxHp > 0.0f) ? (e.hp / e.maxHp) : 0.0f;
        if (pct < 0) pct = 0; if (pct > 1) pct = 1;

        float bw = e.radius * 2.0f;
        float bh = 5.0f;
        float x = e.pos.x - bw * 0.5f;
        float y = e.pos.y - e.radius - 10.0f;

        DrawRectangle((int)x, (int)y, (int)bw, (int)bh, Color{ 30,30,36,220 });
        DrawRectangle((int)x, (int)y, (int)(bw * pct), (int)bh, Color{ 80,230,120,230 });
        DrawRectangleLines((int)x, (int)y, (int)bw, (int)bh, Color{ 220,220,220,200 });
    }
}
