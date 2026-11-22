#pragma once
#include <Geode/Geode.hpp>
#include "Utils.hpp"

namespace GoldCoinProgress {
inline std::string key(int levelID, int index) {
    auto* account = GJAccountManager::get();
    return "gold-collected:" + Utils::serverBaseUrl() + ":"
        + std::to_string(static_cast<int>(account->m_accountID)) + ":"
        + std::to_string(levelID) + ":" + std::to_string(index);
}
inline bool collected(GJGameLevel* level, int index) {
    if (!level || index < 1 || index > 3) return false;
    return geode::Mod::get()->getSavedValue<bool>(key(static_cast<int>(level->m_levelID), index), false);
}
}
