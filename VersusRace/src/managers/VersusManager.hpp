#pragma once

#include <Geode/Geode.hpp>
#include <functional>
#include <string>
#include <unordered_map>

using namespace geode::prelude;

struct VersusSnapshot {
    bool ok = false;
    std::string phase; // idle / searching / matched / ingame / finished
    int matchID = 0;
    int levelID = 0;
    std::string oppName;
    int myLives = 3;
    int oppLives = 3;
    float myPercent = 0.f;
    float oppPercent = 0.f;
    bool youWin = false;
    bool youLose = false;
    int winner = 0;
};

class VersusManager {
public:
    static VersusManager& get();

    bool isSearching() const { return m_searching; }
    bool inMatch() const { return m_matchID > 0 && m_phase != "idle" && m_phase != "finished"; }
    int matchID() const { return m_matchID; }
    int levelID() const { return m_levelID; }
    VersusSnapshot const& last() const { return m_last; }

    void startSearch();
    void cancelSearch();
    void leaveMatch();
    void reportPercent(float percent);
    void reportDeath();
    void pollState();
    void markInGame();

    using Callback = std::function<void(VersusSnapshot const&)>;
    void setListener(Callback cb) { m_listener = std::move(cb); }
    void clearListener() { m_listener = nullptr; }

private:
    VersusManager() = default;

    std::string baseUrl() const;
    std::string endpoint(char const* path) const;
    VersusSnapshot parseJson(matjson::Value const& v) const;
    void applySnapshot(VersusSnapshot const& snap);
    void notify();
    void postAsync(
        char const* path,
        std::unordered_map<std::string, std::string> extra,
        Callback then
    );

    int m_matchID = 0;
    int m_levelID = 0;
    std::string m_phase = "idle";
    VersusSnapshot m_last{};
    Callback m_listener;
    bool m_searching = false;
    bool m_pendingDeath = false;
    unsigned int m_generation = 0;
};
