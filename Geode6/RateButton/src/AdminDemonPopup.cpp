#include "AdminDemonPopup.hpp"
#include "managers/SessionManager.hpp"
#include "managers/StaffRatingClient.hpp"
#include "utils/Utils.hpp"
bool AdminDemonPopup::init(int level) {
    if (!SessionManager::canSetDemon() || !Popup::init(340.f, 165.f)) return false;
    m_level = level; setTitle("Dev: Set Demon", "bigFont.fnt", .75f);
    m_closeBtn->setVisible(false); setID("admin-set-demon-popup"_spr);
    char const* names[] = {"Easy", "Medium", "Hard", "Insane", "Extreme"};
    for (int n = 0; n < 5; ++n) {
        auto* sprite = CCSprite::createWithSpriteFrameName(Utils::getDemonFaceFrame(n));
        auto* button = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(AdminDemonPopup::onPick));
        button->setTag(n); button->setPosition({38.f + n*66.f, 94.f}); m_buttonMenu->addChild(button); m_faces.push_back(button);
        auto* label = CCLabelBMFont::create(names[n], "bigFont.fnt"); label->setScale(.3f);
        label->setPosition({38.f+n*66.f, 59.f}); m_mainLayer->addChild(label);
    }
    auto* cancel = CCMenuItemSpriteExtra::create(ButtonSprite::create("Cancel"), this, menu_selector(AdminDemonPopup::onCancel));
    m_submit = CCMenuItemSpriteExtra::create(ButtonSprite::create("Submit"), this, menu_selector(AdminDemonPopup::onSubmit));
    cancel->setScale(.65f); m_submit->setScale(.65f); cancel->setPosition({110.f,25.f}); m_submit->setPosition({230.f,25.f});
    m_buttonMenu->addChild(cancel); m_buttonMenu->addChild(m_submit); update(); auto weak = WeakRef(this);
    StaffRatingClient::request(level, "read", {}, [weak](bool ok, std::string error, matjson::Value body) {
        auto self = weak.lock(); if (!self) return;
        if (!ok) { FLAlertLayer::create("Could not load", error, "OK")->show(); return; }
        int sub = static_cast<int>(body["subtype"].asInt().unwrapOr(0));
        self->m_selected = sub == 3 ? 0 : sub == 4 ? 1 : sub == 5 ? 3 : sub == 6 ? 4 : 2;
        self->m_ready = true; self->update();
    });
    return true;
}
void AdminDemonPopup::update() {
    for (auto* b : m_faces) b->setOpacity(b->getTag() == m_selected ? 255 : 130);
    bool enabled = m_ready && m_chosen && !m_pending; m_submit->setEnabled(enabled); m_submit->setOpacity(enabled ? 255 : 127);
}
void AdminDemonPopup::onPick(CCObject* sender) {
    if (!m_ready || m_pending) return;
    m_selected = static_cast<CCNode*>(sender)->getTag(); m_chosen = true; update();
}
void AdminDemonPopup::onSubmit(CCObject*) {
    if (!m_ready || !m_chosen || m_pending || !SessionManager::canSetDemon()) return;
    m_pending = true; update(); auto weak = WeakRef(this);
    StaffRatingClient::request(m_level, "demon", {{"difficulty", std::string(Utils::getDemonDifficultyName(m_selected))}},
        [weak, level = m_level](bool ok, std::string error, matjson::Value) {
            auto self = weak.lock(); if (!self) return;
            self->m_pending = false; self->update();
            if (!ok) { FLAlertLayer::create("Could not save", error, "OK")->show(); return; }
            self->onClose(nullptr); Notification::create("Demon difficulty saved", NotificationIcon::Success)->show(); StaffRatingClient::refreshLevel(level);
        });
}
void AdminDemonPopup::onCancel(CCObject*) { onClose(nullptr); }
AdminDemonPopup* AdminDemonPopup::create(int level) {
    auto* p = new AdminDemonPopup(); if (p->init(level)) { p->autorelease(); return p; } delete p; return nullptr;
}
