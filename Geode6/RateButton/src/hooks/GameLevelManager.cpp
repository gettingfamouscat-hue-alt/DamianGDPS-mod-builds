#include <Geode/Geode.hpp>
#include <Geode/modify/GameLevelManager.hpp>
#include "../managers/SessionManager.hpp"
#include "../utils/SearchFilters.hpp"
#include "../utils/Utils.hpp"
#include "../managers/GauntletNames.hpp"

using namespace geode::prelude;

class $modify(GameLevelManager) {
    void onGetGauntletsCompleted(std::string response, std::string tag) {
        GauntletNames::read(response);
        GameLevelManager::onGetGauntletsCompleted(response, tag);
    }
    // Inject GDPS Impossible / Very Easy search params before the request leaves.
    void ProcessHttpRequest(
        std::string endpoint,
        std::string params,
        std::string tag,
        GJHttpType httpType
    ) {
        std::string ep(endpoint);
        if (Utils::isOnGdps()
            && (ep.find("getGJLevels") != std::string::npos
                || httpType == GJHttpType::GetOnlineLevels)) {
            params = SearchFilters::patchLevelSearchParams(params);
        }
        GameLevelManager::ProcessHttpRequest(endpoint, params, tag, httpType);
    }
};
