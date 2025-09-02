#pragma once
#include "Scene.h"
#include "Button.h"
#include "raylib.h"
#include <vector>
#include <string>

class MenuScene : public Scene {
public:
    explicit MenuScene(AppContext& app);
    ~MenuScene() override;
    void Update(float dt) override;
    void Draw() override;

private:
    std::vector<Button> buttons_;
    int selected_ = 0;

    // Background
    Texture2D bg_{};
    bool bgLoaded_ = false;
    std::string bgPath_;
};
