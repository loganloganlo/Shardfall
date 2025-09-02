#pragma once
#include <string>
#include "Profile.h"

bool SaveProfile(const Profile& p, const std::string& path);
bool LoadProfile(Profile& p, const std::string& path);
