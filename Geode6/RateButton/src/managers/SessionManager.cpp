#include <Geode/Geode.hpp>
#include <Geode/binding/GJAccountManager.hpp>
#include <Geode/utils/web.hpp>
#include "SessionManager.hpp"
#include "../utils/Utils.hpp"
#include <chrono>
#include <thread>

using namespace geode::prelude;
namespace {
    int currentAccount = 0;
    std::string currentServer, currentCredential;
    bool manageRoles = false, pending = false, setDemon = false;
    unsigned generation = 0;
    using Clock = std::chrono::steady_clock;
    Clock::time_point verified{}, attempted{};

    void syncContext() {
        auto* am = GJAccountManager::sharedState();
        int account = am ? static_cast<int>(am->m_accountID) : 0;
        std::string credential = am ? std::string(am->m_GJP2) : "";
        std::string server = Utils::isOnGdps() ? Utils::serverBaseUrl() : "";
        if (account != currentAccount || credential != currentCredential || server != currentServer) {
            ++generation;
            currentAccount = account;
            currentCredential = credential;
            currentServer = server;
            SessionManager::ModAccessLevel = 0;
            manageRoles = pending = setDemon = false;
            verified = attempted = {};
        }
        // Cached flags control visibility only. Keep them during transient network
        // failures so the UI does not flicker. Every mutation is authenticated
        // again by its server endpoint; account/server changes clear flags above.

    }
}

bool SessionManager::ShownGDPSWarning = false;
bool SessionManager::ShownRateButtonWarning = false;
int SessionManager::ModAccessLevel = 0;
bool SessionManager::s_loaded = false;

void SessionManager::ensureLoaded() {
    if (!s_loaded) {
        s_loaded = true;
        ShownRateButtonWarning = Mod::get()->getSavedValue<bool>("shown-rate-button-warning", false);
        // Old copied configurations must never carry another account's role.
        Mod::get()->setSavedValue("mod-access-level", 0);
    }
    syncContext();
}

int SessionManager::accountID() { ensureLoaded(); return currentAccount; }
bool SessionManager::canUseRateButton() { ensureLoaded(); return ModAccessLevel >= 1; }
bool SessionManager::isElderMod() { ensureLoaded(); return ModAccessLevel >= 2; }
bool SessionManager::canSetDemon() { ensureLoaded(); return setDemon; }
bool SessionManager::canManageRoles() { ensureLoaded(); return manageRoles; }

void SessionManager::setModAccessLevel(int) {
    // Native badge responses and saved settings are not authorization. Refresh
    // the authenticated GDPS permission endpoint instead.
    refresh();
}

void SessionManager::refresh(bool force) {
    ensureLoaded();
    if (currentAccount <= 0 || currentCredential.empty() || currentServer.empty() || pending) return;
    if (!force && attempted != Clock::time_point{} && Clock::now() - attempted < std::chrono::seconds(5)) return;
    attempted = Clock::now();
    pending = true;
    auto const epoch = generation;
    auto const account = currentAccount;
    auto const credential = currentCredential;
    auto const url = currentServer + "/roles/access.php";
    std::thread([epoch, account, credential, url]() {
        auto request = web::WebRequest();
        request.header("Content-Type", "application/x-www-form-urlencoded");
        request.bodyString("accountID=" + std::to_string(account) + "&gjp2=" + credential);
        request.timeout(asp::time::Duration::fromSecs(8));
        auto response = request.postSync(url);
        Loader::get()->queueInMainThread([epoch, account, response = std::move(response)]() mutable {
            syncContext();
            if (epoch != generation) return;
            pending = false;
            if (!response.ok()) {
                if (response.code() == 401 || response.code() == 403) {
                    ModAccessLevel = 0; manageRoles = setDemon = false;
                }
                return;
            }
            auto parsed = response.json();
            if (!parsed) return;
            auto body = parsed.unwrap();
            if (!body.isObject()) return;
            if (!body["ok"].asBool().unwrapOr(false) || body["accountID"].asInt().unwrapOr(0) != account) return;
            bool const rate = body["canRate"].asBool().unwrapOr(false);
            bool const send = body["canSuggest"].asBool().unwrapOr(false);
            ModAccessLevel = rate ? 2 : (send ? 1 : 0);
            setDemon = body["canSetDemon"].asBool().unwrapOr(false);
            manageRoles = body["canManageRoles"].asBool().unwrapOr(false);
            verified = Clock::now();
        });
    }).detach();
}

void SessionManager::assignRole(int target, std::string role, std::function<void(bool, std::string)> done) {
    ensureLoaded();
    if (!canManageRoles()) { done(false, "Only the server owner can manage roles."); return; }
    auto const epoch = generation;
    auto const account = currentAccount;
    auto const credential = currentCredential;
    auto const url = currentServer + "/roles/set.php";
    std::thread([epoch, account, credential, url, target, role, done = std::move(done)]() mutable {
        auto request = web::WebRequest();
        request.header("Content-Type", "application/x-www-form-urlencoded");
        request.bodyString("accountID=" + std::to_string(account) + "&gjp2=" + credential
            + "&targetAccountID=" + std::to_string(target) + "&role=" + role);
        request.timeout(asp::time::Duration::fromSecs(8));
        auto response = request.postSync(url);
        Loader::get()->queueInMainThread([epoch, target, response = std::move(response), done = std::move(done)]() mutable {
            syncContext();
            if (epoch != generation) { done(false, "Account or server changed. Reopen the player profile."); return; }
            auto parsed = response.json();
            if (!parsed) { done(false, "Role request failed. Please try again."); return; }
            auto body = parsed.unwrap();
            if (!body.isObject()) { done(false, "Invalid permission response."); return; }
            if (!response.ok() || !body["ok"].asBool().unwrapOr(false)) {
                done(false, body["error"].asString().unwrapOr("Role update was denied."));
                return;
            }
            done(true, fmt::format("Role updated for account {}. The player will receive the new permissions automatically.", target));
            SessionManager::refresh(true);
        });
    }).detach();
}
