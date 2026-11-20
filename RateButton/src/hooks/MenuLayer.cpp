#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "../managers/SessionManager.hpp"
#include "../utils/Utils.hpp"

using namespace geode::prelude;

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        SessionManager::ensureLoaded();

        if (Utils::isOnGdps()) {
            // Warm mod access early so LevelInfoLayer already has access>=1.
            GameLevelManager::sharedState()->requestUserAccess();
        } else if (!SessionManager::ShownGDPSWarning) {
            auto *alert = FLAlertLayer::create(
                "Rate Button Disabled",
                "<cj>Rate Button</c> has been <cr>disabled</c> as you are <co>not on a GDPS</c>.",
                "OK"
            );
            alert->m_scene = this;
            alert->show();

            SessionManager::ShownGDPSWarning = true;
        }

        return true;
    }
};