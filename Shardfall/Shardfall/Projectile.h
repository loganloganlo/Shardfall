#pragma once
#include "raylib.h"
#include <vector>

class EnemyManager;
class FloatyText;
class CameraController;

struct Projectile {
    Vector2 pos{ 0,0 };
    Vector2 vel{ 0,0 };
    float   radius = 8.0f;
    float   damage = 20.0f;
    float   lifetime = 1.5f;   // seconds
    Color   color{ 240, 200, 80, 255 };
    bool    alive = true;
};

class ProjectileSystem {
public:
    void Clear() { projs_.clear(); }

    // Generic spawn
    void Spawn(const Vector2& p, const Vector2& v, float radius, float dmg, float life, Color c) {
        Projectile pr; pr.pos = p; pr.vel = v; pr.radius = radius; pr.damage = dmg; pr.lifetime = life; pr.color = c;
        projs_.push_back(pr);
    }

    // Optional named variant (if you keep it)
    void SpawnFireball(const Vector2& from, const Vector2& to);

    // World update/draw
    void Update(float dt, EnemyManager& enemies, FloatyText& fx, CameraController& cam);
    void DrawWorld() const;

    // For debug overlay / HUD
    size_t Count() const { return projs_.size(); }

private:
    std::vector<Projectile> projs_;
};
