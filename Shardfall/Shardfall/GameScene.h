#pragma once
#include "Scene.h"
#include "World.h"
#include "CameraController.h"
#include "Player.h"
#include "Enemy.h"
#include "AbilitySystem.h"
#include "Projectile.h"
#include "FloatyText.h"
#include "Spawner.h"
#include "HUD.h"

class GameScene : public Scene {
public:
    explicit GameScene(AppContext& app);

    void Update(float dt) override;
    void Draw() override;

private:
    World            world_;
    CameraController camera_;
    Player           player_;
    EnemyManager     enemies_;
    ProjectileSystem projectiles_;
    FloatyText       fx_;
    AbilitySystem    abilities_;
    Spawner          spawner_;
    HUD              hud_;

    bool  paused_ = false;
    bool  showOverlay_ = false;

    float playerMaxHp_ = 100.0f;
    float playerHp_ = 100.0f;
    float hurtCd_ = 0.0f;
    float hurtFlash_ = 0.0f; // red flash timer
};
