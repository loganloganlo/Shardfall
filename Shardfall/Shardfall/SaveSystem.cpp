#include "SaveSystem.h"
#include <fstream>
#include <sstream>

static std::string JoinAbilities(const std::vector<AbilityId>& v) {
    std::ostringstream oss;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) oss << ",";
        oss << ToString(v[i]);
    }
    return oss.str();
}

static void SplitCSV(const std::string& s, std::vector<std::string>& out) {
    out.clear();
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, ',')) if (!item.empty()) out.push_back(item);
}

bool SaveProfile(const Profile& p, const std::string& path) {
    std::ofstream f(path, std::ios::trunc);
    if (!f) return false;
    f << "name=" << p.name << "\n";
    f << "class=" << ToString(p.cls) << "\n";
    f << "unlocked=" << JoinAbilities(p.unlocked) << "\n";
    f << "equipped=" << JoinAbilities(p.equipped) << "\n";
    return true;
}

bool LoadProfile(Profile& p, const std::string& path) {
    std::ifstream f(path);
    if (!f) return false;

    std::string line;
    while (std::getline(f, line)) {
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;
        std::string key = line.substr(0, pos);
        std::string val = line.substr(pos + 1);

        if (key == "name") p.name = val;
        else if (key == "class") { ClassId c; if (FromString(val, c)) p.cls = c; }
        else if (key == "unlocked") {
            std::vector<std::string> parts; SplitCSV(val, parts);
            p.unlocked.clear();
            for (auto& s : parts) { AbilityId a; if (FromString(s, a)) p.unlocked.push_back(a); }
        }
        else if (key == "equipped") {
            std::vector<std::string> parts; SplitCSV(val, parts);
            p.equipped.clear();
            for (auto& s : parts) { AbilityId a; if (FromString(s, a)) p.equipped.push_back(a); }
        }
    }
    p.EnsureValidity();
    return true;
}
