#include "Projectile.h"
#include "Enemy.h"             // EnemyManager
#include "raylib.h"
#include <cmath>
#include <algorithm>
#include <cstdio>
#include "FloatyText.h"
#include "CameraController.h"

static inline float Len(const Vector2& v) { return std::sqrt(v.x * v.x + v.y * v.y); }
static inline Vector2 Norm(const Vector2& v) { float l = Len(v); return (l > 0) ? Vector2{ v.x / l, v.y / l } : Vector2{ 0,0 }; }

void ProjectileSystem::SpawnFireball(const Vector2& from, const Vector2& to) {
    Vector2 dir = Norm(Vector2{ to.x - from.x, to.y - from.y });
    float speed = 650.0f;
    float radius = 10.0f;
    float damage = 28.0f;
    float life = 1.6f;
    Color color{ 255, 180, 80, 255 };
    Spawn(from, Vector2{ dir.x * speed, dir.y * speed }, radius, damage, life, color);
}

void ProjectileSystem::Update(float dt, EnemyManager& enemies, FloatyText& fx, CameraController& cam) {
    // Integrate & collide
    for (auto& p : projs_) {
        if (!p.alive) continue;
        p.lifetime -= dt;
        if (p.lifetime <= 0.0f) { p.alive = false; continue; }

        // move
        p.pos.x += p.vel.x * dt;
        p.pos.y += p.vel.y * dt;

        // collide vs enemies
        for (auto& e : enemies.All()) {
            float dx = p.pos.x - e.pos.x;
            float dy = p.pos.y - e.pos.y;
            float rr = p.radius + e.radius;
            if (dx * dx + dy * dy <= rr * rr) {
                enemies.Damage(e, p.damage);
                fx.DamageAt(e.pos, (int)std::round(p.damage));
                cam.AddShake(3.0f);
                p.alive = false;
                break;
            }
        }
    }

    // remove dead
    projs_.erase(std::remove_if(projs_.begin(), projs_.end(),
        [](const Projectile& pr) { return !pr.alive; }), projs_.end());
}

void ProjectileSystem::DrawWorld() const {
    for (const auto& p : projs_) {
        // simple trail
        Vector2 back{ p.pos.x - p.vel.x * 0.03f, p.pos.y - p.vel.y * 0.03f };
        DrawLineEx(back, p.pos, 4.0f, Color{ p.color.r, p.color.g, p.color.b, 140 });
        DrawCircleV(p.pos, p.radius, p.color);
        DrawCircleLines((int)p.pos.x, (int)p.pos.y, p.radius, Color{ 255,255,255,120 });
    }
}
