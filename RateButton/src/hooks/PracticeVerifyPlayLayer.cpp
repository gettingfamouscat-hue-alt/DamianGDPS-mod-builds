#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/binding/LocalLevelManager.hpp>
#include "../utils/Utils.hpp"
#include "../utils/PercentDisplay.hpp"

using namespace geode::prelude;

class $modify(PracticeVerifyPlayLayer, PlayLayer) {
    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);
        if (Utils::isOnGdps() && m_percentageLabel && PercentDisplay::frozen(this->getCurrentPercent())) {
            m_percentageLabel->setString("64%");
        }
    }

    void updateProgressbar() {
        PlayLayer::updateProgressbar();
        if (!Utils::isOnGdps()) return;
        if (PercentDisplay::frozen(this->getCurrentPercent())) {
            if (m_percentageLabel) m_percentageLabel->setString("64%");
            PercentDisplay::capLabels(this);
        } else if (m_lastAttemptPercent >= 64 && m_lastAttemptPercent < 68) {
            PercentDisplay::capLabels(this);
        }
    }

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
