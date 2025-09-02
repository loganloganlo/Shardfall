#pragma once
#include "Scene.h"
#include "Data.h"
#include "Classes.h"
#include "App.h"
#include <vector>

class LoadoutScene : public Scene {
public:
    explicit LoadoutScene(AppContext& app);

    void Update(float dt) override;
    void Draw() override;

private:
    // class selection (left/right or A/D)
    int classIdx_ = 0;                  // index into classes_ vector
    std::vector<ClassId> classes_;      // { Warrior, Mage, Rogue }

    // abilities for current class (up/down select; space/enter toggle equip)
    std::vector<AbilityId> abilities_;
    int abilityCursor_ = 0;

    // helpers
    void RebuildForClass();
    void EnsureDefaultsIfEmpty();
};
