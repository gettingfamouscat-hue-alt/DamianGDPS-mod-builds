#include "StaffRatingClient.hpp"
#include "../utils/Utils.hpp"
#include <Geode/binding/GJAccountManager.hpp>
#include <Geode/utils/web.hpp>
#include <thread>
using namespace geode::prelude;
namespace StaffRatingClient {
namespace {
std::string encode(std::string const& text) {
    std::string result;
    for (unsigned char c : text) {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_') result += c;
        else result += fmt::format("%{:02X}", c);
    }
    return result;
}
}
void request(int level, std::string action, Fields fields, Callback done) {
    auto* am = GJAccountManager::sharedState();
    int account = am->m_accountID;
    std::string credential = am->m_GJP2, server = Utils::serverBaseUrl();
    if (!Utils::isOnGdps() || account <= 0 || credential.empty()) {
        done(false, "Sign in to this GDPS first.", {}); return;
    }
    fields["accountID"] = std::to_string(account); fields["gjp2"] = credential;
    fields["levelID"] = std::to_string(level); fields["action"] = std::move(action);
    std::string body;
    for (auto const& [key, value] : fields) body += (body.empty() ? "" : "&") + encode(key) + "=" + encode(value);
    std::thread([account, credential, server, body, done = std::move(done)]() mutable {
        auto req = web::WebRequest();
        req.header("Content-Type", "application/x-www-form-urlencoded"); req.bodyString(body);
        req.timeout(asp::time::Duration::fromSecs(8));
        auto response = req.postSync(server + "/roles/rating.php");
        Loader::get()->queueInMainThread([account, credential, server, response = std::move(response), done = std::move(done)]() mutable {
            auto* am = GJAccountManager::sharedState();
            if (!Utils::isOnGdps() || am->m_accountID != account || std::string(am->m_GJP2) != credential || Utils::serverBaseUrl() != server) {
                done(false, "Account or server changed. Reopen this menu.", {}); return;
            }
            auto parsed = response.json();
            if (!parsed || !parsed.unwrap().isObject()) { done(false, "Could not reach the rating service. Please try again.", {}); return; }
            auto result = parsed.unwrap();
            bool ok = response.ok() && result["ok"].asBool().unwrapOr(false);
            done(ok, ok ? "Saved" : result["error"].asString().unwrapOr("Rating update denied."), std::move(result));
        });
    }).detach();
}
void refreshLevel(int level) {
    auto* scene = CCDirector::sharedDirector()->getRunningScene();
    if (scene) if (auto* page = scene->getChildByType<LevelInfoLayer>(0))
        if (page->m_level && static_cast<int>(page->m_level->m_levelID) == level) page->downloadLevel();
}
}
