#pragma once
#include "raylib.h"
#include <vector>
#include <unordered_map>
#include <functional>
#include "Data.h"

class AppContext;
class Player;
class EnemyManager;
class World;
class CameraController;
class ProjectileSystem;
class FloatyText;

struct AbilitySlot {
    AbilityId id = AbilityId::None;
    float     cooldown = 0.0f;  // seconds
    float     cdLeft = 0.0f;  // remaining
};

class AbilitySystem {
public:
    AbilitySystem(AppContext& app, Player& player, EnemyManager& enemies, World& world,
        CameraController& cam, ProjectileSystem& projs, FloatyText& fx);

    void BuildFromProfile(); // build registry + equip from profile
    void Update(float dt);
    void DrawWorldOverlay() const;
    void DrawUI() const;
    void HandleInput();

    bool InputBlocked() const { return inputBlocked_; }

private:
    AppContext& app_;
    Player& player_;
    EnemyManager& enemies_;
    World& world_;
    CameraController& cam_;
    ProjectileSystem& projs_;
    FloatyText& fx_;

    bool   inputBlocked_ = false;

    // Equipped bar (LMB/Space = primary handled here too)
    static constexpr int kSlots = 6;
    AbilitySlot slots_[kSlots];

    // --- Ability registry ---
    struct ExecCtx { AbilitySystem& self; };
    struct Def {
        AbilityId id;
        const char* name;
        float cooldown;
        std::function<void(ExecCtx&)> exec;
    };
    std::unordered_map<AbilityId, Def> defs_;
    void RegisterAll(); // 14 Warrior / 14 Mage / 14 Rogue

    // primitives
    void Prim_ConeDamage(float range, float arcDeg, float dmg);
    void Prim_AreaDamage(Vector2 center, float radius, float dmg, float stunSec = 0.0f);
    void Prim_Volley(int count, float spreadDeg, float speed, float life, float radius, float dmg, Color c, bool fromPlayerFacing = true);
    void Prim_Dash(float distance);

    // helpers
    Vector2 FacingDir() const;
    bool InWorld(const Vector2& p) const;
    void PutOnCooldown(AbilityId id);
    Def* GetDef(AbilityId id);
    const Def* GetDef(AbilityId id) const;
};
