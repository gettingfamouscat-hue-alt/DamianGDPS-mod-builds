#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/binding/LocalLevelManager.hpp>
#include "../utils/Utils.hpp"
#include <string>

using namespace geode::prelude;

namespace {
    bool frozenPercent(float percent) {
        return percent >= 63.f && percent < 68.f;
    }

    void freezePercentLabels(CCNode* node) {
        if (!node) return;
        if (auto* label = typeinfo_cast<CCLabelBMFont*>(node)) {
            std::string value = label->getString();
            for (int percent = 64; percent <= 67; ++percent) {
                auto const from = std::to_string(percent) + "%";
                auto const pos = value.find(from);
                if (pos != std::string::npos && (pos == 0 || value[pos - 1] < '0' || value[pos - 1] > '9')) {
                    value.replace(pos, from.size(), "63%");
                    label->setString(value.c_str());
                    break;
                }
            }
        }
        if (node->getChildren()) {
            for (auto* child : node->getChildrenExt()) {
                freezePercentLabels(child);
            }
        }
    }
}

class $modify(PracticeVerifyPlayLayer, PlayLayer) {
    void updateProgressbar() {
        PlayLayer::updateProgressbar();
        if (Utils::isOnGdps() && m_percentageLabel && frozenPercent(this->getCurrentPercent())) {
            m_percentageLabel->setString("63%");
        }
    }

    void showNewBest(bool newReward, int orbs, int diamonds, bool demonKey, bool noRetry, bool noTitle) {
        PlayLayer::showNewBest(newReward, orbs, diamonds, demonKey, noRetry, noTitle);
        if (Utils::isOnGdps() && m_lastAttemptPercent >= 63 && m_lastAttemptPercent < 68) {
            freezePercentLabels(this);
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
