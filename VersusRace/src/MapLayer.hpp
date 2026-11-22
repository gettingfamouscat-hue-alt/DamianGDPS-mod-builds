#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/GameStatsManager.hpp>
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/LevelInfoLayer.hpp>
#include <Geode/utils/web.hpp>
#include "managers/VersusManager.hpp"
#include <chrono>
#include <string>
#include <thread>
#include <vector>

using namespace geode::prelude;

struct MapStop {
    int id;
    std::string name;
};

class DamianMapLayer : public CCLayer {
    std::vector<MapStop> m_stops;
    CCMenu* m_stopsMenu = nullptr;
    CCLabelBMFont* m_status = nullptr;

    void showStops() {
        if (!m_stopsMenu) return;
        m_stopsMenu->removeAllChildrenWithCleanup(true);
        auto win = CCDirector::sharedDirector()->getWinSize();
        auto* stats = GameStatsManager::sharedState();
        for (size_t i = 0; i < m_stops.size(); ++i) {
            bool open = i == 0 || (stats && stats->hasCompletedOnlineLevel(m_stops[i - 1].id));
            bool done = stats && stats->hasCompletedOnlineLevel(m_stops[i].id);
            auto caption = fmt::format("{}  {}", i + 1, open ? m_stops[i].name : "LOCKED");
            auto* sprite = ButtonSprite::create(caption.c_str());
            sprite->setScale(.55f);
            if (done) sprite->setColor({100, 255, 100});
            if (!open) sprite->setColor({140, 140, 140});
            auto* button = CCMenuItemSpriteExtra::create(sprite, this,
                menu_selector(DamianMapLayer::onStop));
            button->setTag(static_cast<int>(i));
            button->setEnabled(open);
            button->setPosition({win.width / 2 + (i % 2 ? 65.f : -65.f),
                win.height - 88.f - static_cast<float>(i) * 51.f});
            m_stopsMenu->addChild(button);
            if (i + 1 < m_stops.size()) {
                auto* arrow = CCLabelBMFont::create("v", "bigFont.fnt");
                arrow->setScale(.35f);
                arrow->setPosition({win.width / 2, win.height - 113.f - static_cast<float>(i) * 51.f});
                m_stopsMenu->addChild(arrow);
            }
        }
        if (m_status) m_status->setString("Finish a level to unlock the next stop");
    }

    void onBack(CCObject*) { CCDirector::sharedDirector()->popScene(); }
    void onStop(CCObject* sender) {
        auto index = static_cast<size_t>(static_cast<CCNode*>(sender)->getTag());
        if (index >= m_stops.size()) return;
        auto* stats = GameStatsManager::sharedState();
        if (index && (!stats || !stats->hasCompletedOnlineLevel(m_stops[index - 1].id))) return;
        auto* level = GameLevelManager::sharedState()->getSavedLevel(m_stops[index].id);
        if (!level) level = GJGameLevel::create();
        level->m_levelID = m_stops[index].id;
        level->m_levelName = m_stops[index].name;
        CCDirector::sharedDirector()->pushScene(LevelInfoLayer::scene(level, false));
    }

    void load() {
        auto url = VersusManager::get().levelDownloadUrl();
        auto slash = url.rfind('/');
        if (slash == std::string::npos || slash == 0) {
            m_status->setString("Set the GDPS URL in Versus settings");
            return;
        }
        url = url.substr(0, slash) + "/getMapPath.php";
        this->retain();
        std::thread([this, url = std::move(url)]() {
            std::vector<MapStop> stops;
            web::WebRequest request;
            request.timeout(std::chrono::seconds(8));
            auto response = request.getSync(url);
            if (response.ok()) {
                auto parsed = response.json();
                if (parsed && parsed.unwrap().isObject()) {
                    auto items = parsed.unwrap()["levels"].asArray();
                    if (items) {
                        for (auto const& entry : items.unwrap()) {
                            int id = static_cast<int>(entry["id"].asInt().unwrapOr(0));
                            auto name = entry["name"].asString().unwrapOr("");
                            if (id > 0 && !name.empty() && stops.size() < 8)
                                stops.push_back({id, name});
                        }
                    }
                }
            }
            Loader::get()->queueInMainThread([this, stops = std::move(stops)]() mutable {
                if (this->getParent()) {
                    m_stops = std::move(stops);
                    if (m_stops.empty()) m_status->setString("Map could not load from this GDPS");
                    else showStops();
                }
                this->release();
            });
        }).detach();
    }

public:
    static DamianMapLayer* create() {
        auto* layer = new DamianMapLayer();
        if (layer && layer->init()) { layer->autorelease(); return layer; }
        delete layer;
        return nullptr;
    }
    static CCScene* scene() {
        auto* scene = CCScene::create();
        if (auto* layer = create()) scene->addChild(layer);
        return scene;
    }
    bool init() override {
        if (!CCLayer::init()) return false;
        setKeypadEnabled(true);
        auto win = CCDirector::sharedDirector()->getWinSize();
        auto* bg = CCLayerColor::create({35, 45, 140, 255});
        addChild(bg, -2);
        auto* title = CCLabelBMFont::create("THE MAP", "bigFont.fnt");
        title->setScale(.8f);
        title->setPosition({win.width / 2, win.height - 31.f});
        addChild(title);
        auto* backMenu = CCMenu::create();
        backMenu->setPosition({0, 0});
        addChild(backMenu);
        auto* back = CCMenuItemSpriteExtra::create(ButtonSprite::create("Back"), this,
            menu_selector(DamianMapLayer::onBack));
        back->setScale(.65f);
        back->setPosition({42.f, win.height - 30.f});
        backMenu->addChild(back);
        m_stopsMenu = CCMenu::create();
        m_stopsMenu->setPosition({0, 0});
        addChild(m_stopsMenu);
        m_status = CCLabelBMFont::create("Loading map...", "bigFont.fnt");
        m_status->setScale(.3f);
        m_status->setPosition({win.width / 2, 19.f});
        addChild(m_status);
        load();
        return true;
    }
    void onEnter() override { CCLayer::onEnter(); if (!m_stops.empty()) showStops(); }
    void keyBackClicked() override { onBack(nullptr); }
};
