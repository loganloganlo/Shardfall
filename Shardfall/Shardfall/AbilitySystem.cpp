#include "AbilitySystem.h"
#include "App.h"
#include "Player.h"
#include "Enemy.h"
#include "World.h"
#include "CameraController.h"
#include "Projectile.h"
#include "FloatyText.h"
#include "Classes.h"
#include <cmath>
#include <algorithm>
#include <cstdio>

AbilitySystem::AbilitySystem(AppContext& app, Player& player, EnemyManager& enemies, World& world,
    CameraController& cam, ProjectileSystem& projs, FloatyText& fx)
    : app_(app), player_(player), enemies_(enemies), world_(world), cam_(cam), projs_(projs), fx_(fx)
{
}

void AbilitySystem::BuildFromProfile() {
    RegisterAll();

    for (int i = 0; i < kSlots; ++i) {
        slots_[i].id = AbilityId::None;
        slots_[i].cooldown = 0.0f;
        slots_[i].cdLeft = 0.0f;
    }
    const auto& eq = app_.profile.equipped;
    for (int i = 0; i < kSlots && i < (int)eq.size(); ++i) {
        slots_[i].id = eq[i];
        if (auto* d = GetDef(eq[i])) slots_[i].cooldown = d->cooldown;
    }
}

void AbilitySystem::Update(float dt) {
    for (int i = 0; i < kSlots; ++i) {
        if (slots_[i].cdLeft > 0.0f) {
            slots_[i].cdLeft -= dt;
            if (slots_[i].cdLeft < 0.0f) slots_[i].cdLeft = 0.0f;
        }
    }
}

void AbilitySystem::DrawWorldOverlay() const {
    // optional debug visuals can go here
}

void AbilitySystem::DrawUI() const {
    const int fs = 18;
    int x = 40, y = GetScreenHeight() - 80;
    DrawText("Abilities:", x, y, fs, RAYWHITE);
    y += 22;
    for (int i = 0; i < kSlots; ++i) {
        const AbilityId id = slots_[i].id;
        char label[96];
        if (id == AbilityId::None) {
            std::snprintf(label, sizeof(label), "%d: (empty)", i + 1);
        }
        else {
            const char* nm = ToString(id);
            if (slots_[i].cdLeft > 0.0f)
                std::snprintf(label, sizeof(label), "%d: %s (%.1fs)", i + 1, nm, slots_[i].cdLeft);
            else
                std::snprintf(label, sizeof(label), "%d: %s", i + 1, nm);
        }
        DrawText(label, x, y + i * 20, fs, LIGHTGRAY);
    }
}

void AbilitySystem::HandleInput() {
    const ClassSpec& cls = GetClassSpec(app_.profile.cls);

    // Primary (LMB / Space) — no UI click sound here
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SPACE)) {
        if (cls.primaryKind == ClassSpec::MeleeCone) {
            Prim_ConeDamage(cls.primaryRange, cls.primaryArcDeg, cls.primaryDamage);
        }
        else {
            Vector2 dir = FacingDir();
            Vector2 vel{ dir.x * cls.primaryProjSpeed, dir.y * cls.primaryProjSpeed };
            projs_.Spawn(player_.pos, vel, cls.primaryProjRadius, cls.primaryProjDamage, cls.primaryProjLife, cls.primaryProjColor);
        }
    }

    // Ability keys 1..6
    for (int i = 0; i < kSlots; ++i) {
        if (IsKeyPressed((KeyboardKey)(KEY_ONE + i))) {
            AbilityId id = slots_[i].id;
            if (id == AbilityId::None) continue;
            if (slots_[i].cdLeft > 0.0f) continue;
            if (auto* d = GetDef(id)) {
                ExecCtx cx{ *this };
                d->exec(cx);           // do the thing (no click SFX)
                PutOnCooldown(id);
            }
        }
    }
}

// ======= primitives =======
Vector2 AbilitySystem::FacingDir() const {
    Vector2 m = cam_.WorldMouse();
    Vector2 d{ m.x - player_.pos.x, m.y - player_.pos.y };
    float len = std::sqrt(d.x * d.x + d.y * d.y);
    if (len <= 0.0001f) return Vector2{ 1,0 };
    return Vector2{ d.x / len, d.y / len };
}

void AbilitySystem::Prim_ConeDamage(float range, float arcDeg, float dmg) {
    Vector2 dir = FacingDir();
    float halfArc = arcDeg * 0.5f;
    float dirAng = std::atan2f(dir.y, dir.x) * RAD2DEG;

    for (auto& e : enemies_.All()) {
        if (!e.alive) continue;
        Vector2 v{ e.pos.x - player_.pos.x, e.pos.y - player_.pos.y };
        float dist = std::sqrt(v.x * v.x + v.y * v.y);
        if (dist > range + e.radius) continue;

        float ang = std::atan2f(v.y, v.x) * RAD2DEG;
        float delta = ang - dirAng;
        while (delta > 180) delta -= 360;
        while (delta < -180) delta += 360;
        if (std::fabs(delta) <= halfArc) {
            enemies_.Damage(e, dmg);
        }
    }
}

void AbilitySystem::Prim_AreaDamage(Vector2 center, float radius, float dmg, float stunSec) {
    for (auto& e : enemies_.All()) {
        if (!e.alive) continue;
        float dx = e.pos.x - center.x;
        float dy = e.pos.y - center.y;
        float rr = radius + e.radius;
        if (dx * dx + dy * dy <= rr * rr) {
            enemies_.Damage(e, dmg);
            if (stunSec > 0.0f) e.stun = std::max(e.stun, stunSec);
        }
    }
}

void AbilitySystem::Prim_Volley(int count, float spreadDeg, float speed, float life, float radius, float dmg, Color c, bool /*fromPlayerFacing*/) {
    Vector2 baseDir = FacingDir();
    float baseAng = std::atan2f(baseDir.y, baseDir.x) * RAD2DEG;

    float startAng = baseAng - spreadDeg * 0.5f;
    float step = (count <= 1) ? 0.0f : (spreadDeg / (count - 1));
    for (int i = 0; i < count; ++i) {
        float ang = startAng + step * i;
        float rad = ang * DEG2RAD;
        Vector2 dir{ std::cos(rad), std::sin(rad) };
        Vector2 vel{ dir.x * speed, dir.y * speed };
        projs_.Spawn(player_.pos, vel, radius, dmg, life, c);
    }
}

void AbilitySystem::Prim_Dash(float distance) {
    Vector2 d = FacingDir();
    Vector2 p{ player_.pos.x + d.x * distance, player_.pos.y + d.y * distance };

    float r = player_.half;
    float minX = world_.bounds.x + r;
    float minY = world_.bounds.y + r;
    float maxX = world_.bounds.x + world_.bounds.width - r;
    float maxY = world_.bounds.y + world_.bounds.height - r;
    if (p.x < minX) p.x = minX;
    if (p.y < minY) p.y = minY;
    if (p.x > maxX) p.x = maxX;
    if (p.y > maxY) p.y = maxY;

    player_.pos = p;
}

void AbilitySystem::PutOnCooldown(AbilityId id) {
    for (int i = 0; i < kSlots; ++i) if (slots_[i].id == id) {
        slots_[i].cdLeft = slots_[i].cooldown;
        break;
    }
}

AbilitySystem::Def* AbilitySystem::GetDef(AbilityId id) {
    auto it = defs_.find(id);
    return (it == defs_.end()) ? nullptr : &it->second;
}
const AbilitySystem::Def* AbilitySystem::GetDef(AbilityId id) const {
    auto it = defs_.find(id);
    return (it == defs_.end()) ? nullptr : &it->second;
}

#define ADD(id, name, cd, body) defs_[id] = Def{ id, name, cd, [=](ExecCtx& ctx){ body } }

void AbilitySystem::RegisterAll() {
    defs_.clear();

    // ======= Warrior (14) =======
    ADD(AbilityId::Warrior_Cleave, "Cleave", 0.60f, ctx.self.Prim_ConeDamage(95, 80, 22); );
    ADD(AbilityId::Warrior_Whirlwind, "Whirlwind", 5.0f, ctx.self.Prim_AreaDamage(ctx.self.player_.pos, 120, 28); );
    ADD(AbilityId::Warrior_ShieldBash, "Shield Bash", 3.5f, ctx.self.Prim_ConeDamage(60, 70, 15); );
    ADD(AbilityId::Warrior_GroundSlam, "Ground Slam", 6.0f, ctx.self.Prim_AreaDamage(ctx.self.player_.pos, 140, 26, 0.6f); );
    ADD(AbilityId::Warrior_Charge, "Charge", 5.0f, ctx.self.Prim_Dash(220); ctx.self.Prim_AreaDamage(ctx.self.player_.pos, 70, 18); );
    ADD(AbilityId::Warrior_Shockwave, "Shockwave", 4.0f, ctx.self.Prim_Volley(1, 0, 900, 0.9f, 9, 22, Color{ 230,220,180,255 }); );
    ADD(AbilityId::Warrior_BattleShout, "Battle Shout", 8.0f, ctx.self.Prim_AreaDamage(ctx.self.player_.pos, 90, 10, 0.2f); );
    ADD(AbilityId::Warrior_Leap, "Leap", 7.0f, ctx.self.Prim_Dash(320); ctx.self.Prim_AreaDamage(ctx.self.player_.pos, 110, 24, 0.2f); );
    ADD(AbilityId::Warrior_Taunt, "Taunt", 10.0f, ctx.self.Prim_AreaDamage(ctx.self.player_.pos, 150, 0, 0.8f); );
    ADD(AbilityId::Warrior_PowerStrike, "Power Strike", 2.5f, ctx.self.Prim_ConeDamage(70, 40, 40); );
    ADD(AbilityId::Warrior_EarthSplit, "Earth Split", 6.5f, ctx.self.Prim_Volley(3, 0, 820, 1.1f, 10, 22, Color{ 220,200,160,255 }); );
    ADD(AbilityId::Warrior_Rally, "Rally", 12.0f, ctx.self.Prim_AreaDamage(ctx.self.player_.pos, 100, 12); );
    ADD(AbilityId::Warrior_Parry, "Parry", 9.0f, ctx.self.Prim_AreaDamage(ctx.self.player_.pos, 60, 6, 0.3f); );
    ADD(AbilityId::Warrior_Berserk, "Berserk", 10.0f, ctx.self.Prim_ConeDamage(100, 100, 26); );

    // ======= Mage (14) =======
    ADD(AbilityId::Mage_Fireball, "Fireball", 0.7f, ctx.self.Prim_Volley(1, 0, 920, 1.2f, 8, 26, Color{ 255,170,70,255 }); );
    ADD(AbilityId::Mage_IceSpike, "Ice Spike", 1.2f, ctx.self.Prim_Volley(2, 10, 900, 1.1f, 7, 18, Color{ 180,220,255,255 }); );
    ADD(AbilityId::Mage_ArcaneNova, "Arcane Nova", 5.0f, ctx.self.Prim_AreaDamage(ctx.self.player_.pos, 140, 28); );
    ADD(AbilityId::Mage_FrostNova, "Frost Nova", 7.0f, ctx.self.Prim_AreaDamage(ctx.self.player_.pos, 120, 10, 0.8f); );
    ADD(AbilityId::Mage_Blink, "Blink", 4.0f, ctx.self.Prim_Dash(260); );
    ADD(AbilityId::Mage_ChainBolt, "Chain Bolt", 4.0f, ctx.self.Prim_Volley(3, 8, 940, 0.9f, 7, 17, Color{ 210,210,255,255 }); );
    ADD(AbilityId::Mage_Meteor, "Meteor", 8.0f, ctx.self.Prim_AreaDamage(ctx.self.cam_.WorldMouse(), 120, 36, 0.2f); );
    ADD(AbilityId::Mage_SlowField, "Slow Field", 10.0f, ctx.self.Prim_AreaDamage(ctx.self.cam_.WorldMouse(), 140, 6, 0.6f); );
    ADD(AbilityId::Mage_MagicMissiles, "Magic Missiles", 3.5f, ctx.self.Prim_Volley(6, 28, 880, 1.0f, 6, 13, Color{ 227,200,255,255 }); );
    ADD(AbilityId::Mage_FlameWave, "Flame Wave", 6.0f, ctx.self.Prim_Volley(7, 60, 820, 0.9f, 7, 15, Color{ 255,160,100,255 }); );
    ADD(AbilityId::Mage_Prisms, "Prism Shot", 5.0f, ctx.self.Prim_Volley(5, 36, 900, 1.0f, 6, 14, Color{ 200,255,255,255 }); );
    ADD(AbilityId::Mage_Spark, "Spark", 1.8f, ctx.self.Prim_ConeDamage(80, 60, 18); );
    ADD(AbilityId::Mage_RuneBomb, "Rune Bomb", 6.5f, ctx.self.Prim_AreaDamage(ctx.self.cam_.WorldMouse(), 150, 30); );
    ADD(AbilityId::Mage_BlizzardBurst, "Blizzard", 9.0f, ctx.self.Prim_AreaDamage(ctx.self.cam_.WorldMouse(), 170, 22, 0.8f); );

    // ======= Rogue (14) =======
    ADD(AbilityId::Rogue_DaggerThrow, "Dagger Throw", 0.45f, ctx.self.Prim_Volley(1, 0, 980, 1.1f, 6, 16, Color{ 255,240,140,255 }); );
    ADD(AbilityId::Rogue_FanKnives, "Fan of Knives", 4.0f, ctx.self.Prim_Volley(10, 120, 900, 0.9f, 6, 14, Color{ 255,230,120,255 }); );
    ADD(AbilityId::Rogue_DashStrike, "Dash Strike", 4.5f, ctx.self.Prim_Dash(260); ctx.self.Prim_ConeDamage(80, 70, 20); );
    ADD(AbilityId::Rogue_SmokeBomb, "Smoke Bomb", 7.0f, ctx.self.Prim_AreaDamage(ctx.self.player_.pos, 110, 8, 0.8f); );
    ADD(AbilityId::Rogue_PoisonCloud, "Poison Cloud", 8.0f, ctx.self.Prim_AreaDamage(ctx.self.cam_.WorldMouse(), 140, 18); );
    ADD(AbilityId::Rogue_Trap, "Trap", 6.5f, ctx.self.Prim_AreaDamage(ctx.self.cam_.WorldMouse(), 120, 22); );
    ADD(AbilityId::Rogue_Shadowstep, "Shadowstep", 5.0f, ctx.self.Prim_Dash(300); );
    ADD(AbilityId::Rogue_Backstab, "Backstab", 3.0f, ctx.self.Prim_ConeDamage(60, 50, 34); );
    ADD(AbilityId::Rogue_Shiv, "Shiv", 1.2f, ctx.self.Prim_ConeDamage(55, 60, 14); );
    ADD(AbilityId::Rogue_Shuriken, "Shuriken Volley", 1.8f, ctx.self.Prim_Volley(3, 14, 960, 1.0f, 6, 14, Color{ 255,240,160,255 }); );
    ADD(AbilityId::Rogue_Net, "Net", 5.0f, ctx.self.Prim_Volley(1, 0, 820, 1.2f, 9, 6, Color{ 230,230,255,255 }); );
    ADD(AbilityId::Rogue_Cripple, "Cripple", 6.0f, ctx.self.Prim_AreaDamage(ctx.self.player_.pos, 100, 10, 0.6f); );
    ADD(AbilityId::Rogue_ExplosiveVial, "Explosive Vial", 6.5f, ctx.self.Prim_AreaDamage(ctx.self.cam_.WorldMouse(), 130, 24); );
    ADD(AbilityId::Rogue_DecoyBurst, "Decoy Burst", 9.0f, ctx.self.Prim_AreaDamage(ctx.self.player_.pos, 140, 20, 0.2f); );
}

#undef ADD
