#pragma once
#include "raylib.h"
#include <vector>
#include <string>

class FloatyText {
public:
    void Clear() { items_.clear(); }
    void Update(float dt);
    void DrawWorld() const;

    void DamageAt(Vector2 worldPos, int amount, bool crit = false);

private:
    struct Item {
        Vector2 pos;
        float   t = 0.0f;      // elapsed
        float   ttl = 0.8f;    // lifetime
        float   vy = -38.0f;   // upward velocity (px/s)
        std::string text;
        Color   color;
    };
    std::vector<Item> items_;
};
