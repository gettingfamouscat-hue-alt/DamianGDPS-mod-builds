#include <Geode/Geode.hpp>
#include <Geode/binding/GJAccountManager.hpp>
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/utils/web.hpp>
#include "../managers/VersusManager.hpp"
#include "../MapLayer.hpp"

#include <chrono>
#include <functional>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace geode::prelude;

namespace {
    void enterLevel(GJGameLevel* level) {
        if (!level)
            return;
        VersusManager::get().markInGame();
        auto* scene = PlayLayer::scene(level, false);
        CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, scene));
    }

    GJGameLevel* parseVersusLevel(std::string const& response, int levelID) {
        auto end = response.find('#');
        auto fields = response.substr(0, end);
        std::unordered_map<std::string, std::string> values;
        size_t start = 0;
        while (start < fields.size()) {
            auto separator = fields.find(':', start);
            if (separator == std::string::npos) break;
            auto next = fields.find(':', separator + 1);
            auto key = fields.substr(start, separator - start);
            auto value = fields.substr(separator + 1,
                next == std::string::npos ? std::string::npos : next - separator - 1);
            values.emplace(std::move(key), std::move(value));
            if (next == std::string::npos) break;
            start = next + 1;
        }
        if (values["1"] != std::to_string(levelID) || values["4"].empty())
            return nullptr;

        auto* dict = CCDictionary::create();
        for (auto const& [key, value] : values)
            dict->setObject(CCString::create(value.c_str()), key.c_str());
        auto* level = GJGameLevel::create(dict, true);
        if (!level || static_cast<int>(level->m_levelID) != levelID || level->m_levelString.empty())
            return nullptr;
        return level;
    }

    void playVersusLevel(int levelID) {
        auto url = VersusManager::get().levelDownloadUrl();
        if (url == "/downloadGJLevel22.php") {
            Notification::create("Set the GDPS URL in Versus settings", NotificationIcon::Error)->show();
            VersusManager::get().leaveMatch();
            return;
        }
        Notification::create("Downloading versus level…", NotificationIcon::Info)->show();
        std::thread([url = std::move(url), levelID]() {
            web::WebRequest req;
            req.header("Content-Type", "application/x-www-form-urlencoded");
            req.bodyString(fmt::format("levelID={}&gameVersion=22&binaryVersion=38", levelID));
            req.timeout(asp::time::Duration::fromSecs(20));
            auto res = req.postSync(url);
            auto body = res.ok() ? res.string().unwrapOr("") : std::string{};
            Loader::get()->queueInMainThread([levelID, body = std::move(body)]() {
                if (!VersusManager::get().inMatch() || VersusManager::get().levelID() != levelID)
                    return;
                if (auto* level = parseVersusLevel(body, levelID)) {
                    enterLevel(level);
                } else {
                    log::warn("Versus: GDPS level {} download failed", levelID);
                    Notification::create("Versus level could not load", NotificationIcon::Error)->show();
                    VersusManager::get().leaveMatch();
                }
            });
        }).detach();
    }

    void onMatched(VersusSnapshot const& snap) {
        if (snap.matchID <= 0 || snap.levelID <= 0)
            return;
        createQuickPopup(
            "Versus",
            fmt::format(
                "Matched vs {}!\nLevel ID {}\n3 lives — first to finish wins, 0 lives loses.",
                snap.oppName,
                snap.levelID
            ),
            "Cancel",
            "Play",
            [snap](FLAlertLayer*, bool btn2) {
                if (!btn2) {
                    VersusManager::get().leaveMatch();
                    return;
                }
                playVersusLevel(snap.levelID);
            }
        );
    }

    std::string spriteFrameName(CCNode* node) {
        auto* spr = typeinfo_cast<CCSprite*>(node);
        if (!spr)
            return {};
        auto* frame = spr->displayFrame();
        if (!frame)
            return {};
        return std::string(frame->getFrameName());
    }

    bool nodeLooksLikeVersusSprite(CCNode* node) {
        auto name = spriteFrameName(node);
        if (name.find("GJ_versusBtn") != std::string::npos ||
            name.find("versusBtn") != std::string::npos) {
            return true;
        }
        if (auto* children = node->getChildren()) {
            for (auto* child : CCArrayExt<CCNode*>(children)) {
                if (nodeLooksLikeVersusSprite(child))
                    return true;
            }
        }
        return false;
    }

    bool isLockOverlay(CCNode* node) {
        if (!node)
            return false;
        std::string id = node->getID();
        if (id.find("lock") != std::string::npos)
            return true;
        auto name = spriteFrameName(node);
        return name.find("GJ_lock") != std::string::npos ||
               name.find("lockGray") != std::string::npos ||
               name.find("lock_001") != std::string::npos;
    }

    void restoreFullColor(CCNode* node) {
        if (!node)
            return;
        if (auto* sprite = typeinfo_cast<CCSprite*>(node)) {
            sprite->setShaderProgram(CCShaderCache::sharedShaderCache()->programForKey(
                kCCShader_PositionTextureColor));
        }
        if (auto* rgba = typeinfo_cast<CCRGBAProtocol*>(node)) {
            rgba->setColor(ccc3(255, 255, 255));
            rgba->setOpacity(255);
        }
        if (auto* children = node->getChildren()) {
            // Collect lock overlays first so we don't mutate while iterating
            std::vector<CCNode*> locks;
            for (auto* child : CCArrayExt<CCNode*>(children)) {
                if (isLockOverlay(child))
                    locks.push_back(child);
                else
                    restoreFullColor(child);
            }
            for (auto* lock : locks) {
                lock->removeFromParent();
            }
        }
    }

    CCMenuItemSpriteExtra* findVersusButton(CCNode* root) {
        if (!root)
            return nullptr;

        if (auto* menu = root->getChildByID("creator-buttons-menu")) {
            if (auto* btn = typeinfo_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("versus-button")))
                return btn;
        }

        // Fallback without node-ids: locate by versus sprite frame name
        std::function<CCMenuItemSpriteExtra*(CCNode*)> walk;
        walk = [&](CCNode* node) -> CCMenuItemSpriteExtra* {
            if (auto* btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                if (std::string(btn->getID()) == "versus-button" || nodeLooksLikeVersusSprite(btn))
                    return btn;
            }
            if (auto* children = node->getChildren()) {
                for (auto* child : CCArrayExt<CCNode*>(children)) {
                    if (auto* found = walk(child))
                        return found;
                }
            }
            return nullptr;
        };
        return walk(root);
    }

    void enableVersusButton(CCNode* root) {
        auto* btn = findVersusButton(root);
        if (!btn)
            return;

        btn->setEnabled(true);
        btn->setVisible(true);
        restoreFullColor(btn);
        if (auto* spr = btn->getNormalImage())
            restoreFullColor(spr);
    }

    CCMenuItemSpriteExtra* findMapButton(CCNode* root) {
        if (!root) return nullptr;
        std::function<bool(CCNode*)> containsMapSprite;
        containsMapSprite = [&](CCNode* node) {
            if (!node) return false;
            if (spriteFrameName(node) == "GJ_mapBtn_001.png") return true;
            if (auto* children = node->getChildren())
                for (auto* child : CCArrayExt<CCNode*>(children))
                    if (containsMapSprite(child)) return true;
            return false;
        };
        std::function<CCMenuItemSpriteExtra*(CCNode*)> walk;
        walk = [&](CCNode* node) -> CCMenuItemSpriteExtra* {
            if (auto* button = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                auto* image = button->getNormalImage();
                if (std::string(button->getID()) == "map-button" ||
                    std::string(button->getID()) == "adventure-map-button" ||
                    containsMapSprite(image) || containsMapSprite(button)) return button;
            }
            if (auto* children = node->getChildren())
                for (auto* child : CCArrayExt<CCNode*>(children))
                    if (auto* found = walk(child)) return found;
            return nullptr;
        };
        return walk(root);
    }

    void enableMapButton(CCNode* root) {
        auto* button = findMapButton(root);
        if (!button) { log::warn("Map tile was not found"); return; }
        button->setEnabled(true);
        restoreFullColor(button);
        if (auto* image = button->getNormalImage()) restoreFullColor(image);
    }
}

class $modify(VersusCreatorLayer, CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init())
            return false;
        enableVersusButton(this);
        enableMapButton(this);
        return true;
    }

    void sceneWillResume() {
        CreatorLayer::sceneWillResume();
        enableVersusButton(this);
        enableMapButton(this);
    }

    void onAdventureMap(CCObject*) {
        CCDirector::sharedDirector()->pushScene(DamianMapLayer::scene());
    }

    void onMultiplayer(CCObject* sender) {
        if (VersusManager::get().isSearching()) {
            VersusManager::get().clearListener();
            VersusManager::get().cancelSearch();
            Notification::create("Search cancelled", NotificationIcon::Warning)->show();
            return;
        }

        auto* am = GJAccountManager::sharedState();
        if (!am || am->m_accountID <= 0) {
            FLAlertLayer::create("Versus", "Log in to your GDPS account first.", "OK")->show();
            return;
        }

        createQuickPopup(
            "Versus",
            "Do you want to find a match?",
            "No",
            "Yes",
            [](FLAlertLayer*, bool yes) {
                if (!yes)
                    return;

                Notification::create(
                    "Searching… (open Versus again to cancel)",
                    NotificationIcon::Info
                )->show();

                VersusManager::get().setListener([](VersusSnapshot const& snap) {
                    if (snap.phase == "matched" && snap.matchID > 0) {
                        VersusManager::get().clearListener();
                        Notification::create("Match found!", NotificationIcon::Success)->show();
                        onMatched(snap);
                    }
                });
                VersusManager::get().startSearch();
            }
        );
    }
};
