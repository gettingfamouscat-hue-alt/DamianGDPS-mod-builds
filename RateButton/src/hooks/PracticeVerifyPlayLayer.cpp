#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/binding/LocalLevelManager.hpp>
#include "../utils/Utils.hpp"

using namespace geode::prelude;

class $modify(PracticeVerifyPlayLayer, PlayLayer) {
    void levelComplete() {
        // Verification applies only to levels created locally on this GDPS.
        // Keep practice completion out of normal-mode progress and rewards.
        bool verifyInPractice = Utils::isOnGdps() && m_isPracticeMode && m_level
            && m_level->m_levelType == GJLevelType::Editor;
        PlayLayer::levelComplete();
        if (!verifyInPractice || !m_level) return;

        m_level->m_isVerified = true;
        m_level->m_isVerifiedRaw = true;
        if (auto* levels = LocalLevelManager::get()) levels->save();
    }
};
