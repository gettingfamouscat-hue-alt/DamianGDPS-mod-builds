#include "CoinContext.hpp"
#include <km7dev.server_api/ServerAPIEvents.hpp>
#include <algorithm>
#include <cctype>
using namespace geode::prelude;

int GoldenCoin::accountID() {
    auto* account = GJAccountManager::get();
    return account ? static_cast<int>(account->m_accountID) : 0;
}

std::string GoldenCoin::serverBaseUrl() {
    auto url = Mod::get()->getSettingValue<std::string>("server-url");
    if (url.empty()) {
        if (auto* previous = Loader::get()->getLoadedMod("m336.rate-button")) {
            if (previous->hasSetting("server-url"))
                url = previous->getSettingValue<std::string>("server-url");
        }
    }
    if (url.empty() && Loader::get()->isModLoaded("km7dev.server_api")) {
        auto current = ServerAPIEvents::getCurrentServer().url;
        if (!current.empty() && current != "NONE_REGISTERED") url = current;
    }
    if (url.empty()) url = "http://192.168.0.143/gdps";
    while (!url.empty() && url.back() == '/') url.pop_back();
    if (url.ends_with("/database")) url.resize(url.size() - 9);
    return url;
}

bool GoldenCoin::isOnGdps() {
    if (Loader::get()->isModLoaded("km7dev.server_api")) {
        auto url = ServerAPIEvents::getCurrentServer().url;
        if (url.empty() || url == "NONE_REGISTERED") url = ServerAPIEvents::getBaseUrl();
        if (!url.empty() && url != "NONE_REGISTERED") {
            auto scheme = url.find("://");
            auto begin = scheme == std::string::npos ? 0 : scheme + 3;
            auto end = url.find_first_of("/:?#", begin);
            auto host = url.substr(begin, end - begin);
            std::transform(host.begin(), host.end(), host.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            return !(host == "boomlings.com" || host.ends_with(".boomlings.com")
                || host == "geometrydash.com" || host.ends_with(".geometrydash.com"));
        }
    }
    return Mod::get()->getSettingValue<bool>("custom-gdps-mode");
}
