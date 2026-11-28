#include "FeaturedOrderPopup.hpp"
#include "CustomStarsPopup.hpp"
#include "managers/StaffRatingClient.hpp"
#include "managers/SessionManager.hpp"
#include <algorithm>
#include <limits>
bool FeaturedOrderPopup::init(int level) {
    if (!SessionManager::isElderMod() || !Popup::init(400.f, 235.f)) return false;
    m_level = level; setTitle("Set Featured", "bigFont.fnt", .85f);
    setID("featured-order-popup"_spr);
    auto text = [this](char const* value, CCPoint point, float scale) {
        auto* l = CCLabelBMFont::create(value, "bigFont.fnt"); l->setScale(scale); l->setPosition(point); m_mainLayer->addChild(l); return l;
    };
    text("Higher order appears first in Featured", {200.f, 176.f}, .32f);
    m_number = CCLabelBMFont::create("Loading...", "bigFont.fnt");
    auto* number = CCMenuItemSpriteExtra::create(m_number, this, menu_selector(FeaturedOrderPopup::onNumber));
    number->setPosition({200.f, 123.f}); m_buttonMenu->addChild(number);
    int steps[] = {-100, -1, 1, 100}; float xs[] = {40.f, 90.f, 310.f, 360.f};
    for (int n = 0; n < 4; ++n) {
        auto* sprite = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
        sprite->setRotation(n < 2 ? -90.f : 90.f); sprite->setScale(.8f);
        if (n == 0 || n == 3) sprite->setColor({155, 255, 80});
        auto* b = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(FeaturedOrderPopup::onStep));
        b->setTag(steps[n]); b->setPosition({xs[n], 123.f}); m_buttonMenu->addChild(b);
        text(n == 0 ? "-100" : n == 1 ? "-1" : n == 2 ? "+1" : "+100", {xs[n], 93.f}, .24f);
    }
    m_fire = CCSprite::createWithSpriteFrameName("GJ_featuredCoin_001.png"); m_fire->setScale(.7f);
    auto* fire = CCMenuItemSpriteExtra::create(m_fire, this, menu_selector(FeaturedOrderPopup::onFire));
    fire->setPosition({142.f, 62.f}); m_buttonMenu->addChild(fire);
    m_fireText = text("Not featured", {240.f, 62.f}, .36f);
    auto* cancel = CCMenuItemSpriteExtra::create(ButtonSprite::create("Cancel"), this, menu_selector(FeaturedOrderPopup::onCancel));
    m_submit = CCMenuItemSpriteExtra::create(ButtonSprite::create("Submit"), this, menu_selector(FeaturedOrderPopup::onSubmit));
    cancel->setScale(.7f); m_submit->setScale(.7f);
    cancel->setPosition({135.f, 25.f}); m_submit->setPosition({265.f, 25.f});
    m_buttonMenu->addChild(cancel); m_buttonMenu->addChild(m_submit); update();
    auto weak = WeakRef(this);
    StaffRatingClient::request(level, "read", {}, [weak](bool ok, std::string error, matjson::Value body) {
        auto self = weak.lock(); if (!self) return;
        if (!ok) { FLAlertLayer::create("Could not load", error, "OK")->show(); return; }
        self->m_order = static_cast<int>(body["order"].asInt().unwrapOr(0));
        self->m_tier = std::clamp(static_cast<int>(body["tier"].asInt().unwrapOr(0)), 0, 4);
        self->m_ready = true; self->update();
    });
    return true;
}
void FeaturedOrderPopup::update() {
    if (m_ready) { m_number->setString(std::to_string(m_order).c_str()); m_number->limitLabelWidth(175.f, 1.f, .35f); }
    char const* frames[] = {"GJ_featuredCoin_001.png", "GJ_featuredCoin_001.png", "GJ_epicCoin_001.png", "GJ_epicCoin2_001.png", "GJ_epicCoin3_001.png"};
    char const* names[] = {"Not featured", "Featured", "Epic", "Legendary", "Mythic"};
    m_fire->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frames[m_tier]));
    m_fire->setOpacity(m_tier ? 255 : 100); m_fireText->setString(names[m_tier]);
    m_submit->setEnabled(m_ready && !m_pending); m_submit->setOpacity(m_ready && !m_pending ? 255 : 127);
}
void FeaturedOrderPopup::onStep(CCObject* sender) {
    if (!m_ready || m_pending) return;
    m_order = static_cast<int>(std::clamp<int64_t>(static_cast<int64_t>(m_order) + static_cast<CCNode*>(sender)->getTag(), 0, std::numeric_limits<int>::max()));
    m_tier = m_order ? std::max(1, m_tier) : 0; update();
}
void FeaturedOrderPopup::onNumber(CCObject*) {
    if (!m_ready || m_pending) return;
    auto weak = WeakRef(this);
    if (auto* popup = CustomStarsPopup::create(m_order, [weak](int value) {
        if (auto self = weak.lock()) { self->m_order = value; self->m_tier = value ? std::max(1, self->m_tier) : 0; self->update(); }
    }, true)) popup->show();
}
void FeaturedOrderPopup::onFire(CCObject*) {
    if (!m_ready || m_pending) return;
    m_tier = (m_tier + 1) % 5;
    if (m_tier) m_order = std::max(1, m_order); else m_order = 0;
    update();
}
void FeaturedOrderPopup::onSubmit(CCObject*) {
    if (!m_ready || m_pending) return;
    m_pending = true; update(); auto weak = WeakRef(this);
    StaffRatingClient::request(m_level, "featured", {{"order", std::to_string(m_order)}, {"tier", std::to_string(m_tier)}},
        [weak, level = m_level](bool ok, std::string error, matjson::Value) {
            auto self = weak.lock(); if (!self) return;
            self->m_pending = false; self->update();
            if (!ok) { FLAlertLayer::create("Could not save", error, "OK")->show(); return; }
            self->onClose(nullptr); Notification::create("Featured order saved", NotificationIcon::Success)->show(); StaffRatingClient::refreshLevel(level);
        });
}
void FeaturedOrderPopup::onCancel(CCObject*) { onClose(nullptr); }
FeaturedOrderPopup* FeaturedOrderPopup::create(int level) {
    auto* p = new FeaturedOrderPopup(); if (p->init(level)) { p->autorelease(); return p; } delete p; return nullptr;
}
