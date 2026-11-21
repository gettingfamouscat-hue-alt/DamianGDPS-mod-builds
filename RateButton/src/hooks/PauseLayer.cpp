#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include "../utils/PercentDisplay.hpp"
#include "../utils/Utils.hpp"

using namespace geode::prelude;

class $modify(GdpsPercentPauseLayer, PauseLayer) {
    void setupProgressBars() {
        PauseLayer::setupProgressBars();
        if (!Utils::isOnGdps()) return;
        auto* play = PlayLayer::get();
        if (play && play->m_level && PercentDisplay::frozen(static_cast<int>(play->m_level->m_normalPercent))) {
            PercentDisplay::capExactLabels(this);
        }
    }
};
