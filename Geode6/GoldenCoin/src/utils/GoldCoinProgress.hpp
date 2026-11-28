#pragma once
#include <Geode/Geode.hpp>
#include "CoinContext.hpp"

namespace GoldCoinProgress {
inline std::string key(int levelID, int index) {
    auto* account = GJAccountManager::get();
    return "gold-collected:" + GoldenCoin::serverBaseUrl() + ":"
        + std::to_string(static_cast<int>(account->m_accountID)) + ":"
        + std::to_string(levelID) + ":" + std::to_string(index);
}
inline bool collected(GJGameLevel* level, int index) {
    if (!level || index < 1 || index > 3) return false;
    auto savedKey = key(static_cast<int>(level->m_levelID), index);
    auto* mod = geode::Mod::get();
    if (mod->getSavedValue<bool>(savedKey, false)) return true;
    if (auto* legacy = geode::Loader::get()->getLoadedMod("m336.rate-button")) {
        if (legacy->getSavedValue<bool>(savedKey, false)) {
            mod->setSavedValue(savedKey, true);
            return true;
        }
    }
    return false;
}
}
