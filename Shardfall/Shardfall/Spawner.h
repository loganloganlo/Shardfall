#pragma once
#include "raylib.h"

class EnemyManager;

class Spawner {
public:
    void Init(float bossAtSeconds = 180.0f);
    void Update(float dt, EnemyManager& enemies, const Rectangle& worldBounds,
        const Vector2& playerPos, bool paused);

    void DrawHUD() const;

    float GetRunTimeSec() const { return runTimeSec_; }
    int   GetWave()       const { return waveIndex_ + 1; }
    float GetBossTimeSec()const { return bossTimeSec_; }
    bool  BossSpawned()   const { return bossSpawned_; }

    // public so HUD/GameScene can format times
    static void FormatMMSS(char out[8], float t);

private:
    float runTimeSec_ = 0.0f;
    int   waveIndex_ = -1;
    float nextWaveTimer_ = 0.0f;
    float trickleTimer_ = 0.0f;
    bool  bossSpawned_ = false;
    float bossTimeSec_ = 180.0f;

    float wavePeriod_ = 22.0f;
    int   baseWaveCount_ = 6;
    int   perWaveAdd_ = 3;
    float tricklePeriod_ = 6.0f;
    float spawnEdgeMargin_ = 48.0f;

    void LaunchNextWave(EnemyManager& enemies, const Rectangle& worldBounds, const Vector2& playerPos);
    void SpawnTrickle(EnemyManager& enemies, const Rectangle& worldBounds, const Vector2& playerPos);
    void TrySpawnBoss(EnemyManager& enemies, const Rectangle& worldBounds, const Vector2& playerPos);

    static Vector2 RandomEdgePoint(const Rectangle& world, float margin);
};
