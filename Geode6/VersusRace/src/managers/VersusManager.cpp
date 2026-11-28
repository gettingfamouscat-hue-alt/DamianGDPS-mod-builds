#include "VersusManager.hpp"

#include <Geode/binding/GJAccountManager.hpp>
#include <Geode/utils/web.hpp>

#include <chrono>
#include <thread>

#if __has_include(<km7dev.server_api/ServerAPIEvents.hpp>)
#include <km7dev.server_api/ServerAPIEvents.hpp>
#define VERSUS_HAS_SERVER_API 1
#else
#define VERSUS_HAS_SERVER_API 0
#endif

VersusManager& VersusManager::get() {
    static VersusManager inst;
    return inst;
}

std::string VersusManager::baseUrl() const {
    std::string url = Mod::get()->getSettingValue<std::string>("server-url");
    while (!url.empty() && url.back() == '/')
        url.pop_back();

#if VERSUS_HAS_SERVER_API
    if (url.empty() && Loader::get()->isModLoaded("km7dev.server_api")) {
        auto cur = ServerAPIEvents::getCurrentServer().url;
        if (!cur.empty() && cur != "NONE_REGISTERED") {
            while (!cur.empty() && cur.back() == '/')
                cur.pop_back();
            if (cur.size() >= 9 && cur.substr(cur.size() - 9) == "/database")
                cur = cur.substr(0, cur.size() - 9);
            if (!cur.empty())
                url = cur;
        }
    }
#endif
    return url;
}

std::string VersusManager::endpoint(char const* path) const {
    return fmt::format("{}/versus/{}", this->baseUrl(), path);
}

std::string VersusManager::levelDownloadUrl() const {
    return fmt::format("{}/downloadGJLevel22.php", this->baseUrl());
}

VersusSnapshot VersusManager::parseJson(matjson::Value const& v) const {
    VersusSnapshot s;
    s.ok = v.contains("ok") ? v["ok"].asBool().unwrapOr(true) : true;
    if (v.contains("phase"))
        s.phase = v["phase"].asString().unwrapOr("idle");
    else if (v.contains("status")) {
        auto st = v["status"].asString().unwrapOr("active");
        s.phase = (st == "finished") ? "finished" : "matched";
    }
    s.matchID = static_cast<int>(v["matchID"].asInt().unwrapOr(0));
    s.levelID = static_cast<int>(v["levelID"].asInt().unwrapOr(0));
    if (v.contains("you")) {
        auto you = v["you"];
        auto icon=you["icon"];
        if(icon.isObject()) {s.myIcon=static_cast<int>(icon["icon"].asInt().unwrapOr(1));
            s.myColor1=static_cast<int>(icon["color1"].asInt().unwrapOr(0));
            s.myColor2=static_cast<int>(icon["color2"].asInt().unwrapOr(3));}
        s.myLives = static_cast<int>(you["lives"].asInt().unwrapOr(3));
        s.myPercent = static_cast<float>(you["percent"].asDouble().unwrapOr(0.0));
    }
    if (v.contains("opponent")) {
        auto opp = v["opponent"];
        auto icon=opp["icon"];
        if(icon.isObject()) {s.oppIcon=static_cast<int>(icon["icon"].asInt().unwrapOr(1));
            s.oppColor1=static_cast<int>(icon["color1"].asInt().unwrapOr(0));
            s.oppColor2=static_cast<int>(icon["color2"].asInt().unwrapOr(3));}
        s.oppName = opp["userName"].asString().unwrapOr("?");
        s.oppLives = static_cast<int>(opp["lives"].asInt().unwrapOr(3));
        s.oppPercent = static_cast<float>(opp["percent"].asDouble().unwrapOr(0.0));
    }
    s.winner = static_cast<int>(v["winner"].asInt().unwrapOr(0));
    s.youWin = v.contains("youWin") ? v["youWin"].asBool().unwrapOr(false) : false;
    s.youLose = v.contains("youLose") ? v["youLose"].asBool().unwrapOr(false) : false;
    return s;
}

void VersusManager::applySnapshot(VersusSnapshot const& snap) {
    m_last = snap;
    if (!snap.phase.empty())
        m_phase = snap.phase;
    if (snap.matchID > 0)
        m_matchID = snap.matchID;
    if (snap.levelID > 0)
        m_levelID = snap.levelID;
    if (snap.phase == "finished" || snap.youWin || snap.youLose) {
        m_phase = "finished";
        m_searching = false;
    }
    this->notify();
}

void VersusManager::notify() {
    auto listener = m_listener;
    if (listener) listener(m_last);
}

void VersusManager::postAsync(
    char const* path,
    std::unordered_map<std::string, std::string> extra,
    Callback then
) {
    auto* am = GJAccountManager::sharedState();
    if (!am || am->m_accountID <= 0) {
        log::warn("Versus: not logged in");
        VersusSnapshot fail;
        fail.ok = false;
        fail.phase = "idle";
        if (then) then(fail);
        return;
    }

    auto url = this->endpoint(path);
    int accountID = static_cast<int>(am->m_accountID);
    std::string gjp2 = std::string(am->m_GJP2);
    auto const generation = m_generation;

    std::thread([this, url, accountID, gjp2, generation, extra = std::move(extra), then = std::move(then)]() mutable {
        auto req = web::WebRequest();
        std::string form = "accountID=" + std::to_string(accountID) + "&gjp2=" + gjp2;
        for (auto const& [k, v] : extra) form += "&" + k + "=" + v;
        req.header("Content-Type", "application/x-www-form-urlencoded");
        req.bodyString(form);
        req.timeout(asp::time::Duration::fromSecs(8));

        auto res = req.postSync(url);
        Loader::get()->queueInMainThread([this, generation, res = std::move(res), then = std::move(then)]() mutable {
            if (generation != m_generation) return;
            if (!res.ok()) {
                log::warn("Versus: HTTP {} body={}", res.code(), res.string().unwrapOr(""));
                VersusSnapshot fail;
                fail.ok = false;
                fail.phase = m_phase;
                fail.matchID = m_matchID;
                fail.levelID = m_levelID;
                if (then) then(fail);
                return;
            }
            auto jsonRes = res.json();
            if (!jsonRes) {
                log::warn("Versus: bad JSON {}", res.string().unwrapOr(""));
                VersusSnapshot fail;
                fail.ok = false;
                if (then) then(fail);
                return;
            }
            auto snap = this->parseJson(jsonRes.unwrap());
            this->applySnapshot(snap);
            if (then) then(snap);
        });
    }).detach();
}

void VersusManager::startSearch() {
    ++m_generation;
    m_pendingDeath = false;
    m_phase = "searching";
    m_matchID = 0;
    m_levelID = 0;
    m_searching = true;
    m_last = {};
    m_last.phase = "searching";
    this->notify();

    auto poll = std::make_shared<std::function<void()>>();
    *poll = [this, poll]() {
        if (!m_searching)
            return;
        this->postAsync("find.php", {}, [this, poll](VersusSnapshot const& snap) {
            if (!m_searching)
                return;
            if (snap.phase == "matched" && snap.matchID > 0) {
                m_searching = false;
                return;
            }
            std::thread([this, poll]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(900));
                Loader::get()->queueInMainThread([this, poll]() {
                    if (m_searching)
                        (*poll)();
                });
            }).detach();
        });
    };
    (*poll)();
}

void VersusManager::cancelSearch() {
    ++m_generation;
    m_searching = false;
    this->postAsync("cancel.php", {}, [this](VersusSnapshot const&) {
        m_phase = "idle";
        m_matchID = 0;
        m_levelID = 0;
        m_last = {};
        m_last.phase = "idle";
        this->notify();
    });
}

void VersusManager::leaveMatch() {
    ++m_generation;
    m_pendingDeath = false;
    m_searching = false;
    std::unordered_map<std::string, std::string> extra;
    if (m_matchID > 0)
        extra["matchID"] = std::to_string(m_matchID);
    this->postAsync("leave.php", extra, [this](VersusSnapshot const&) {
        m_phase = "idle";
        m_matchID = 0;
        m_levelID = 0;
        m_last = {};
        this->notify();
    });
}

void VersusManager::markInGame() {
    if (m_phase == "matched")
        m_phase = "ingame";
}

void VersusManager::reportPercent(float percent) {
    if (m_matchID <= 0)
        return;
    this->postAsync("state.php", {
        {"matchID", std::to_string(m_matchID)},
        {"percent", fmt::format("{:.2f}", percent)},
    }, nullptr);
}

void VersusManager::reportDeath() {
    if (m_matchID <= 0 || m_pendingDeath)
        return;
    m_pendingDeath = true;
    this->postAsync("state.php", {
        {"matchID", std::to_string(m_matchID)},
        {"death", "1"},
    }, [this](VersusSnapshot const&) {
        m_pendingDeath = false;
    });
}

void VersusManager::pollState() {
    if (m_matchID <= 0)
        return;
    this->postAsync("state.php", {
        {"matchID", std::to_string(m_matchID)},
    }, nullptr);
}
