#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include "MapLayer.hpp"
#include <functional>
#include <vector>
using namespace geode::prelude;
namespace {
    std::string spriteFrameName(CCNode* node) {
        auto* spr = typeinfo_cast<CCSprite*>(node);
        if (!spr)
            return {};
        auto* frame = spr->displayFrame();
        if (!frame)
            return {};
        return std::string(frame->getFrameName());
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
class $modify(DamianMapCreatorLayer, CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init()) return false;
        enableMapButton(this); return true;
    }
    void sceneWillResume() {
        CreatorLayer::sceneWillResume(); enableMapButton(this);
    }
    void onAdventureMap(CCObject*) {
        CCDirector::sharedDirector()->pushScene(DamianMapLayer::scene());
    }
};
