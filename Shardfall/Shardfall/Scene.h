#pragma once
#include "App.h"

enum class SceneRequest { None, Menu, Loadout, Game, Settings, Quit };

class Scene {
public:
    explicit Scene(AppContext& app) : app_(app) {}
    virtual ~Scene() = default;
    virtual void Update(float dt) = 0;
    virtual void Draw() = 0;

    SceneRequest ConsumeRequest() {
        SceneRequest r = request_;
        request_ = SceneRequest::None;
        return r;
    }

protected:
    void Request(SceneRequest r) { request_ = r; }
    AppContext& app_;

private:
    SceneRequest request_{ SceneRequest::None };
};
