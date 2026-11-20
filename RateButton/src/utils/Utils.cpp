#include "Utils.hpp"
#include <km7dev.server_api/include/ServerAPIEvents.hpp>
#include <algorithm>
#include <cctype>

using namespace geode::prelude;

bool Utils::isOnGdps() {
    if (Loader::get()->isModLoaded("km7dev.server_api")) {
        std::string url = ServerAPIEvents::getCurrentServer().url;
        if (!url.empty() && url != "NONE_REGISTERED") {
            auto scheme = url.find("://");
            auto begin = scheme == std::string::npos ? 0 : scheme + 3;
            auto end = url.find_first_of("/:?#", begin);
            std::string host = url.substr(begin, end - begin);
            std::transform(host.begin(), host.end(), host.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            return !(host == "boomlings.com" || host.ends_with(".boomlings.com")
                || host == "geometrydash.com" || host.ends_with(".geometrydash.com"));
        }
    }
    // Patched Mac clients need not register with Server API. Never read an
    // unverified platform-specific memory offset to detect a private server.
    return Mod::get()->getSettingValue<bool>("custom-gdps-mode");
}
