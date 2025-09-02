#include "Profile.h"
#include "Classes.h"   // GetClassSpec, GetAbilitiesForClass
#include <algorithm>

static inline void clamp01(float& x) {
    if (x < 0.0f) x = 0.0f;
    if (x > 1.0f) x = 1.0f;
}

void Profile::EnsureValidity() {
    // clamp volumes
    clamp01(musicVolume);
    clamp01(sfxVolume);

    // allowed abilities for the selected class
    std::vector<AbilityId> allowed = GetAbilitiesForClass(cls);

    // if unlocked is empty, assume all allowed are unlocked (starter profile)
    if (unlocked.empty()) unlocked = allowed;

    // dedupe unlocked
    std::sort(unlocked.begin(), unlocked.end());
    unlocked.erase(std::unique(unlocked.begin(), unlocked.end()), unlocked.end());

    // filter equipped -> must be both allowed and unlocked
    auto isAllowedUnlocked = [&](AbilityId a) {
        return std::find(allowed.begin(), allowed.end(), a) != allowed.end() &&
            std::binary_search(unlocked.begin(), unlocked.end(), a);
        };
    equipped.erase(std::remove_if(equipped.begin(), equipped.end(),
        [&](AbilityId a) { return !isAllowedUnlocked(a); }),
        equipped.end());

    // dedupe equipped, keep order
    std::vector<AbilityId> dedup;
    for (auto a : equipped) {
        if (std::find(dedup.begin(), dedup.end(), a) == dedup.end())
            dedup.push_back(a);
    }
    equipped.swap(dedup);

    // cap to 6
    if (equipped.size() > 6) equipped.resize(6);

    // ensure at least some defaults if empty
    if (equipped.empty()) {
        const auto& spec = GetClassSpec(cls);
        // prefer defaults that are also unlocked
        for (auto a : spec.defaultEquipped) {
            if (std::find(unlocked.begin(), unlocked.end(), a) != unlocked.end())
                equipped.push_back(a);
            if (equipped.size() >= 6) break;
        }
        // still empty? just take first few allowed
        for (auto a : allowed) {
            if (equipped.size() >= 6) break;
            if (std::find(equipped.begin(), equipped.end(), a) == equipped.end())
                equipped.push_back(a);
        }
    }
}
