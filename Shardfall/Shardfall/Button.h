#pragma once
#include "raylib.h"
#include <string>

struct Button {
    Rectangle rect;
    std::string label;
    int fontSize;

    Button(Rectangle r, const std::string& t, int sz = 28)
        : rect(r), label(t), fontSize(sz) {
    }

    bool Update() const {
        Vector2 m = GetMousePosition();
        bool hover = CheckCollisionPointRec(m, rect);
        return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }

    void Draw(bool focused = false) const {
        Vector2 m = GetMousePosition();
        bool hover = CheckCollisionPointRec(m, rect);
        bool active = hover || focused;

        DrawRectangleRounded(rect, 0.2f, 8, active ? Fade(DARKGRAY, 0.9f) : Fade(DARKGRAY, 0.7f));
        DrawRectangleRoundedLines(rect, 0.2f, 8, active ? RAYWHITE : LIGHTGRAY);

        int tw = MeasureText(label.c_str(), fontSize);
        float tx = rect.x + (rect.width - tw) * 0.5f;
        float ty = rect.y + (rect.height - fontSize) * 0.5f - 2.0f;
        DrawText(label.c_str(), (int)tx, (int)ty, fontSize, RAYWHITE);
    }
};
