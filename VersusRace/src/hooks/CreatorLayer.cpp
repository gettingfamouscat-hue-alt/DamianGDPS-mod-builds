#include <Geode/Geode.hpp>
#include <Geode/binding/GJAccountManager.hpp>
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include "../managers/VersusManager.hpp"

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <vector>

using namespace geode::prelude;

namespace {
    void enterLevel(GJGameLevel* level) {
        if (!level)
            return;
        VersusManager::get().markInGame();
        auto* scene = PlayLayer::scene(level, false, false);
        CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, scene));
    }

    void playVersusLevel(int levelID) {
        auto* glm = GameLevelManager::sharedState();
        if (!glm)
            return;

        if (auto* saved = glm->getSavedLevel(levelID)) {
            enterLevel(saved);
            return;
        }

        glm->downloadLevel(levelID, false, 0);
        Notification::create("Downloading versus level…", NotificationIcon::Info)->show();

        // Poll until the level is cached (onDownloadLevelCompleted is inline / unhookable)
        auto finished = std::make_shared<std::atomic_bool>(false);
        std::thread([levelID, finished]() {
            for (int i = 0; i < 40; i++) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                if (finished->load())
                    return;
                Loader::get()->queueInMainThread([levelID, finished]() {
                    if (finished->load())
                        return;
                    if (!VersusManager::get().inMatch()) {
                        finished->store(true);
                        return;
                    }
                    auto* glm = GameLevelManager::sharedState();
                    if (!glm)
                        return;
                    if (auto* lvl = glm->getSavedLevel(levelID)) {
                        finished->store(true);
                        enterLevel(lvl);
                    }
                });
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(600));
            if (!finished->load()) {
                Loader::get()->queueInMainThread([]() {
                    Notification::create("Versus download timed out", NotificationIcon::Error)->show();
                });
            }
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
}

class $modify(VersusCreatorLayer, CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init())
            return false;
        enableVersusButton(this);
        return true;
    }

    void sceneWillResume() {
        CreatorLayer::sceneWillResume();
        enableVersusButton(this);
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
