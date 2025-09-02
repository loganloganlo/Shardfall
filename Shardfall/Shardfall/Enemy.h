#pragma once
#include "raylib.h"
#include <vector>

struct Enemy {
    Vector2 pos{ 0,0 };
    Vector2 vel{ 0,0 };
    float   speed = 110.0f;
    float   radius = 16.0f;
    float   hp = 60.0f;
    float   maxHp = 60.0f;   // for health bar %
    float   stun = 0.0f;
    bool    alive = true;
    Color   color = Color{ 200, 50, 70, 255 };
};

class EnemyManager {
public:
    void Clear();

    // Update AI & cull dead enemies
    void Update(float dt, Vector2 playerPos, const Rectangle& bounds);

    // Draw world
    void Draw() const;

    // Access
    std::vector<Enemy>& All() { return enemies_; }
    const std::vector<Enemy>& All() const { return enemies_; }

    // Damage helper
    void Damage(Enemy& e, float amount);

    // Spawns used by Spawner
    void SpawnBasic(Vector2 pos);
    void SpawnBoss(Vector2 pos);

private:
    std::vector<Enemy> enemies_;
};
