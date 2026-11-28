#define GEODE_DEFINE_EVENT_EXPORTS
#include <ranges>
#include <Geode/Geode.hpp>
#include "ServerAPI.hpp"
#include "../include/ServerAPIEvents.hpp"
#include "Geode/cocos/support/zip_support/ZipUtils.h"
#include "VoidCast.hpp"

using namespace ServerAPIEvents;
using namespace geode::prelude;

constexpr int REQUIRED_GD_VERSION = 22091;

const StringMap<ServerAPITrust::TrustLevel> ServerAPI::m_trustedModsLUT = {
    {"lblazen.gdps_hub", ServerAPITrust::TrustLevel::HighlyTrusted},
    {"km7dev.gdps-switcher", ServerAPITrust::TrustLevel::HighlyTrusted} // Putting my own mod here because I have an ego and fuck you.
};

ServerAPI* ServerAPI::instance = nullptr;

int ServerAPI::registerURL(std::string url, int priority) {
    if (!url.ends_with("/")) url += "/";
    // <id, <url, prio>>
    this->m_overrides[this->m_nextId] = std::pair(std::move(url), priority);
    this->m_nextId++;
    evaluateCache();
    return this->m_nextId - 1;
}

void ServerAPI::updateURLAndPrio(int id, std::string url, int priority) {
    if (!m_overrides.contains(id)) {
        log::error("Attempted to update priority of server ({}) which does not exist", id);
        return;
    }
    if (!url.ends_with("/")) url = url + "/";
    this->m_overrides[id] = std::pair(std::move(url), priority);
    evaluateCache();
}

void ServerAPI::updateURL(int id, std::string url) {
    if (!m_overrides.contains(id)) {
        log::error("Attempted to update priority of server ({}) which does not exist", id);
        return;
    }
    if (!url.ends_with("/")) url = url + "/";
    m_overrides[id].first = std::move(url);
    evaluateCache(); // TODO: Maybe cache should use references to the servers?
}

void ServerAPI::updatePrio(int id, int priority) {
    if (!m_overrides.contains(id)) {
        log::error("Attempted to update priority of server ({}) which does not exist", id);
        return;
    }
    m_overrides[id].second = priority;
    evaluateCache();
}

std::string ServerAPI::getURLById(int id) {
    for (const auto& [uid, urlPrio] : m_overrides) {
        if (id == uid) {
            return urlPrio.first;
        }
    }
    return "";
}

int ServerAPI::getPrioById(int id) {
    for (const auto& [uid, urlPrio] : m_overrides) {
        if (id == uid) {
            return urlPrio.second;
        }
    }
    return 0;
}

std::string ServerAPI::getCurrentURL() {
    return m_cache.URL;
}

int ServerAPI::getCurrentPrio() {
    return m_cache.Priority;
}

int ServerAPI::getCurrentId() {
    return m_cache.ID;
}

const StringMap<ServerAPITrust::TrustLevel>& ServerAPI::getTrustedMods() {
    return m_trustedModsLUT;
}

void ServerAPI::removeURL(int id) {
    this->m_overrides.erase(id);
    if (m_overrides.empty()) {
        m_cache = ServerCache::none();
    }
}

std::map<int, std::pair<std::string, int>> ServerAPI::getAllServers() {
    return m_overrides;
}

std::string ServerAPI::getBaseUrl() {
    return m_baseUrl;
}

std::string ServerAPI::getSecondaryUrl() {
    return m_secondaryUrl;
}

void ServerAPI::init() {
    m_amazon = false;
    static_assert(GEODE_COMP_GD_VERSION == REQUIRED_GD_VERSION, "Unsupported GD version");
    // Requests are rewritten by the HTTP hooks. Do not read URL strings
    // through 2.2081 executable offsets after a game update.
    this->m_baseUrl = "https://www.boomlings.com/database/";
    this->m_secondaryUrl = "http://www.boomlings.com/database/";
    m_cache = ServerCache::none();

    // Trims endpoint off of URL.
    if (this->m_baseUrl.size() > 36)
        this->m_baseUrl = this->m_baseUrl.substr(0, 35);
    
    if (this->m_secondaryUrl.size() > 35)
        this->m_secondaryUrl = this->m_secondaryUrl.substr(0, 34);

    // Overrides web requests to base servers from other mods (Requires them to have a cookie __SERVERAPI_OVERRIDE_URL=1)
    m_webRequestInterceptor = web::WebRequestInterceptEvent().listen([](web::WebRequest& req) noexcept -> ListenerResult {
        bool shouldOverride = false;
        auto headers = req.getHeaders();
        // Out of range exception if we don't do this
        if (!headers.contains("Cookie")) {
            return ListenerResult::Propagate;
        }

        for (const auto& cookie : headers.at("Cookie")) {
            if (cookie != "__SERVERAPI_OVERRIDE_URL=1") continue;
            shouldOverride = true;
        }
        if (!shouldOverride) return ListenerResult::Propagate;

        std::string url = req.getUrl();
        if (url.starts_with(ServerAPI::get()->getBaseUrl())) {
            url.replace(0, ServerAPI::get()->getBaseUrl().size(), ServerAPI::get()->getCurrentURL());
        } else if (url.starts_with(ServerAPI::get()->getSecondaryUrl())) {
            url.replace(0, ServerAPI::get()->getSecondaryUrl().size(), ServerAPI::get()->getCurrentURL());
        } else {
            return ListenerResult::Propagate;
        }
        req.url(std::move(url));

        return ListenerResult::Propagate;
    });
}

void ServerAPI::evaluateCache() {
    std::pair<std::string, int> highestServer = std::make_pair("NONE_REGISTERED", INT_MIN);
    int highestServerId = 0;
    
    for (const auto& [id, server] : m_overrides) {
        const auto& [url, prio] = server;
        if (prio > highestServer.second) {
            highestServerId = id;
            highestServer = server;
        }
    }

    m_cache.ID = highestServerId;
    m_cache.URL = highestServer.first;
    m_cache.Priority = highestServer.second;
}

ServerAPI* ServerAPI::get() {
    if (!instance) {
        instance = new ServerAPI();
        instance->init();
    }
    return instance;
};

template <typename CFunc, typename... TArgs>
requires std::invocable<CFunc, ServerAPI*, TArgs...> // Must be method of ServerAPI
decltype(auto) ServerAPI::doAndNotifyIfServerUpdate(CFunc&& func, Mod* updater, TArgs&&... args) {
    int oldID = m_cache.ID;

    // Void is an incomplete type, so we have to hack our way around it
    using TRet = VoidCastT<std::invoke_result_t<CFunc, ServerAPI*, TArgs...>>;
    using TCastBack = std::invoke_result_t<CFunc, ServerAPI*, TArgs...>;
    TRet ret = safeInvoke(
        std::forward<CFunc>(func),
        this,
        std::forward<TArgs>(args)...
    );

    // Server has updated
    if (oldID != m_cache.ID) {
        ServerUpdatingEvent().send(updater, m_cache.URL);
    }

    // Cast back to void if original result type is void
    return static_cast<TCastBack>(ret);
}

bool ServerAPI::isAmazon() {
    return m_amazon;
}

Server ServerAPIEvents::getCurrentServer() {
    return {
        .id = ServerAPI::get()->getCurrentId(),
        .url = ServerAPI::get()->getCurrentURL(),
        .priority = ServerAPI::get()->getCurrentPrio()
    };
}

Server ServerAPIEvents::getServerById(int id) {
    return {
        .id = id,
        .url = ServerAPI::get()->getURLById(id),
        .priority = ServerAPI::get()->getPrioById(id)
    };
}

Server ServerAPIEvents::registerServer(std::string url, int priority, Mod* sender) {
    int id = ServerAPI::get()->doAndNotifyIfServerUpdate(&ServerAPI::registerURL, sender, url, priority);
    return {
        .id = id,
        .url = std::move(url),
        .priority = priority
    };
}

void ServerAPIEvents::updateServer(int id, std::string url, int priority, Mod* sender) {
    if (url != "") {
        if (priority != 0) {
            ServerAPI::get()->doAndNotifyIfServerUpdate(&ServerAPI::updateURLAndPrio, sender, id, std::move(url), priority);
        } else {
            ServerAPI::get()->doAndNotifyIfServerUpdate(&ServerAPI::updateURL, sender, id, std::move(url));
        }
    } else {
        ServerAPI::get()->doAndNotifyIfServerUpdate(&ServerAPI::updatePrio, sender, id, priority);
    }
}

void ServerAPIEvents::removeServer(int id, Mod* sender) {
    ServerAPI::get()->doAndNotifyIfServerUpdate(&ServerAPI::removeURL, sender, id);
}

std::string ServerAPIEvents::getBaseUrl() {
    return ServerAPI::get()->getBaseUrl();
}

std::string ServerAPIEvents::getSecondaryUrl() {
    return ServerAPI::get()->getSecondaryUrl();
}

std::map<int, std::pair<std::string, int>> ServerAPIEvents::getRegisteredServers() {
    return ServerAPI::get()->getAllServers();
}

StringMap<ServerAPITrust::TrustLevel> ServerAPITrust::getTrustedMods() {
    return ServerAPI::get()->getTrustedMods();
}
