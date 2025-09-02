#include "Spawner.h"
#include "Enemy.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

void Spawner::Init(float bossAtSeconds) {
    runTimeSec_ = 0.0f;
    waveIndex_ = -1;
    nextWaveTimer_ = 0.0f;
    trickleTimer_ = tricklePeriod_;
    bossSpawned_ = false;
    bossTimeSec_ = bossAtSeconds;
}

void Spawner::Update(float dt, EnemyManager& enemies, const Rectangle& world, const Vector2& playerPos, bool paused) {
    if (paused) return;

    runTimeSec_ += dt;

    TrySpawnBoss(enemies, world, playerPos);

    if (waveIndex_ < 0) {
        LaunchNextWave(enemies, world, playerPos);
    }
    else {
        nextWaveTimer_ -= dt;
        if (nextWaveTimer_ <= 0.0f) {
            LaunchNextWave(enemies, world, playerPos);
        }
    }

    trickleTimer_ -= dt;
    if (trickleTimer_ <= 0.0f) {
        SpawnTrickle(enemies, world, playerPos);
        tricklePeriod_ = std::max(2.5f, tricklePeriod_ - 0.05f);
        trickleTimer_ = tricklePeriod_;
    }
}

void Spawner::DrawHUD() const {
    char timeTxt[8];  FormatMMSS(timeTxt, runTimeSec_);
    char bossTxt[8];  FormatMMSS(bossTxt, bossTimeSec_);

    const int fs = 28;
    char line[128];
    std::snprintf(line, sizeof(line), "Time %s   |   Wave %d   |   Boss at %s%s",
        timeTxt, std::max(0, waveIndex_ + 1), bossTxt, bossSpawned_ ? " (spawned)" : "");

    int tw = MeasureText(line, fs);
    DrawText(line, (GetScreenWidth() - tw) / 2, 12, fs, RAYWHITE);
}

void Spawner::LaunchNextWave(EnemyManager& enemies, const Rectangle& world, const Vector2& /*playerPos*/) {
    waveIndex_++;
    nextWaveTimer_ = wavePeriod_;

    int count = baseWaveCount_ + waveIndex_ * perWaveAdd_;

    for (int i = 0; i < count; ++i) {
        Vector2 p = RandomEdgePoint(world, spawnEdgeMargin_);
        enemies.SpawnBasic(p);
    }
}

void Spawner::SpawnTrickle(EnemyManager& enemies, const Rectangle& world, const Vector2& /*playerPos*/) {
    int n = (GetRandomValue(0, 100) < 30) ? 2 : 1;
    for (int i = 0; i < n; ++i) {
        Vector2 p = RandomEdgePoint(world, spawnEdgeMargin_);
        enemies.SpawnBasic(p);
    }
}

void Spawner::TrySpawnBoss(EnemyManager& enemies, const Rectangle& world, const Vector2& /*playerPos*/) {
    if (bossSpawned_ || runTimeSec_ < bossTimeSec_) return;
    bossSpawned_ = true;
    Vector2 p = RandomEdgePoint(world, spawnEdgeMargin_);
    enemies.SpawnBoss(p);
}

Vector2 Spawner::RandomEdgePoint(const Rectangle& w, float m) {
    int edge = GetRandomValue(0, 3);

    if (edge == 0) { // top
        float x = static_cast<float>(GetRandomValue(static_cast<int>(w.x + m),
            static_cast<int>(w.x + w.width - m)));
        return Vector2{ x, w.y + m };

    }
    else if (edge == 1) { // bottom
        float x = static_cast<float>(GetRandomValue(static_cast<int>(w.x + m),
            static_cast<int>(w.x + w.width - m)));
        return Vector2{ x, w.y + w.height - m };

    }
    else if (edge == 2) { // left
        float y = static_cast<float>(GetRandomValue(static_cast<int>(w.y + m),
            static_cast<int>(w.y + w.height - m)));
        return Vector2{ w.x + m, y };

    }
    else { // right
        float y = static_cast<float>(GetRandomValue(static_cast<int>(w.y + m),
            static_cast<int>(w.y + w.height - m)));
        return Vector2{ w.x + w.width - m, y };
    }
}

void Spawner::FormatMMSS(char out[8], float t) {
    if (t < 0) t = 0;
    int tt = (int)std::floor(t + 0.5f);
    int m = tt / 60;
    int s = tt % 60;
    std::snprintf(out, 8, "%02d:%02d", m, s);
}
