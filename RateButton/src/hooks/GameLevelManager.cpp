#include <Geode/Geode.hpp>
#include <Geode/modify/GameLevelManager.hpp>
#include "../managers/SessionManager.hpp"
#include "../utils/SearchFilters.hpp"
#include "../utils/Utils.hpp"

using namespace geode::prelude;

static int parseModAccessResponse(gd::string const& response) {
    std::string s(response);
    while (!s.empty() && (s.back() == '\n' || s.back() == '\r' || s.back() == ' ' || s.back() == '\t'))
        s.pop_back();
    while (!s.empty() && (s.front() == '\n' || s.front() == '\r' || s.front() == ' ' || s.front() == '\t'))
        s.erase(s.begin());

    if (s == "2")
        return 2;
    if (s == "1")
        return 1;
    // Some servers prefix / suffix junk — accept leading digit
    if (!s.empty() && s[0] == '2')
        return 2;
    if (!s.empty() && s[0] == '1')
        return 1;
    return 0;
}

class $modify(GameLevelManager) {
    void onRequestUserAccessCompleted(gd::string response, gd::string tag) {
        GameLevelManager::onRequestUserAccessCompleted(response, tag);

        auto const level = parseModAccessResponse(response);
        log::info("Rate Button: requestUserAccess raw='{}' parsed={}", response, level);

        // Always store — even if isOnGdps check is flaky at this moment
        SessionManager::setModAccessLevel(level);
    }

    // Inject GDPS Impossible / Very Easy search params before the request leaves.
    void ProcessHttpRequest(
        gd::string endpoint,
        gd::string params,
        gd::string tag,
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
