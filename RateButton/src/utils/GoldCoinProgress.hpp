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
    if (geode::Mod::get()->getSavedValue<bool>(key(static_cast<int>(level->m_levelID), index), false)) return true;
    // Preserve coins collected before the dedicated gold-coin tracking existed.
    auto coinKey = level->getCoinKey(index);
    return GameStatsManager::get()->hasUserCoin(coinKey)
        || GameStatsManager::get()->hasPendingUserCoin(coinKey);
}
}
