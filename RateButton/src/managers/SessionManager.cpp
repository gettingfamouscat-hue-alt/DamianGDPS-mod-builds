#include "SessionManager.hpp"

using namespace geode::prelude;

bool SessionManager::ShownGDPSWarning = false;
bool SessionManager::ShownRateButtonWarning = false;
int SessionManager::ModAccessLevel = 0;
bool SessionManager::s_loaded = false;

void SessionManager::ensureLoaded() {
    if (s_loaded)
        return;
    s_loaded = true;
    ShownRateButtonWarning = Mod::get()->getSavedValue<bool>("shown-rate-button-warning", false);
    ModAccessLevel = Mod::get()->getSavedValue<int>("mod-access-level", 0);
}

bool SessionManager::canUseRateButton() {
    ensureLoaded();
    return ModAccessLevel >= 1;
}

bool SessionManager::isElderMod() {
    ensureLoaded();
    return ModAccessLevel >= 2;
}

void SessionManager::setModAccessLevel(int level) {
    ensureLoaded();
    if (level < 0) level = 0;
    if (level > 2) level = 2;
    ModAccessLevel = level;
    Mod::get()->setSavedValue("mod-access-level", level);
    log::info("Rate Button: mod access level -> {}", level);
}
