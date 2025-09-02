#include "FloatyText.h"
#include "raylib.h"
#include <algorithm>
#include <cstdio>

void FloatyText::DamageAt(Vector2 worldPos, int amount, bool crit) {
    Item it;
    it.pos = worldPos;
    it.ttl = crit ? 1.1f : 0.8f;
    it.vy = crit ? -48.0f : -38.0f;
    it.text = std::to_string(amount);
    it.color = crit ? Color{ 255,230,120,255 } : Color{ 230,80,80,255 };
    items_.push_back(it);
}

void FloatyText::Update(float dt) {
    for (auto& it : items_) {
        it.t += dt;
        it.pos.y += it.vy * dt;
    }
    items_.erase(std::remove_if(items_.begin(), items_.end(),
        [](const Item& i) { return i.t >= i.ttl; }), items_.end());
}

void FloatyText::DrawWorld() const {
    for (const auto& it : items_) {
        float a = 1.0f - (it.t / it.ttl);
        if (a < 0) a = 0; if (a > 1) a = 1;
        Color c = it.color; c.a = (unsigned char)(a * 255);

        int fs = 20;
        int tw = MeasureText(it.text.c_str(), fs);
        DrawText(it.text.c_str(), (int)(it.pos.x - tw * 0.5f), (int)(it.pos.y - fs * 0.5f), fs, c);
    }
}
