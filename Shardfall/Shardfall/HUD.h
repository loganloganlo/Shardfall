#pragma once
#include "raylib.h"

// include real headers to keep 'struct/class' consistent across TUs
#include "App.h"
#include "Player.h"
#include "Enemy.h"
#include "Projectile.h"
#include "Spawner.h"

class HUD {
public:
    explicit HUD(AppContext& app);

    void Draw(const Player& player, float playerHp, float playerMaxHp,
        const EnemyManager& enemies, const ProjectileSystem& projectiles,
        const Spawner& spawner) const;

    void DrawOverlay(const Player& player, float playerHp, float playerMaxHp,
        const EnemyManager& enemies, const ProjectileSystem& projectiles,
        const Spawner& spawner) const;

private:
    AppContext& app_;

    void DrawPlayerHP(float hp, float maxHp) const;
    void DrawRunWaveBoss(const Spawner& spawner) const;
};
