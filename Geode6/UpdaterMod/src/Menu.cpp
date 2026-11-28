#include "Updater.hpp"
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/TextArea.hpp>
using namespace geode::prelude;
namespace {
class UpdatePopup : public Popup {
    std::shared_ptr<updater::State> m_state;
    SimpleTextArea* m_status=nullptr;
    CCMenuItemSpriteExtra* m_cancel=nullptr;
    bool m_finished=false;
    bool init() override {
        if (!Popup::init(340.f,170.f)) return false;
        setTitle("Auto Update"); m_closeBtn->setVisible(false);
        m_status=SimpleTextArea::create("Checking for updates...", "chatFont.fnt", .65f, 290.f);
        m_status->setMaxLines(4); m_status->setAlignment(kCCTextAlignmentCenter);
        m_status->setPosition({170.f,95.f}); m_mainLayer->addChild(m_status);
        auto* note=CCLabelBMFont::create("Your saves and settings stay.","chatFont.fnt");
        note->setScale(.55f); note->setPosition({170.f,51.f}); m_mainLayer->addChild(note);
        m_cancel=CCMenuItemSpriteExtra::create(ButtonSprite::create("Cancel"),this,menu_selector(UpdatePopup::cancel));
        m_cancel->setScale(.55f); m_cancel->setPosition({170.f,24.f}); m_buttonMenu->addChild(m_cancel);
        m_state=updater::start(); if (!m_state) return false;
        schedule(schedule_selector(UpdatePopup::tick),.1f); return true;
    }
    void cancel(CCObject*) {
        if (m_finished) { onClose(nullptr); return; }
        int expected=0;
        if (!m_state->phase.compare_exchange_strong(expected,1)) return; m_cancel->setEnabled(false);
        m_status->setText("Cancelling the download...");
    }
    void tick(float) {
        bool finished=m_state->done.load();
        std::string text,error;
        { std::lock_guard lock(m_state->mutex); text=m_state->message; error=m_state->error; }
        if (m_state->phase!=1) {
            auto percent=m_state->percentage.load();
            if (percent>=0) text+="\n"+std::to_string(percent)+"%";
            m_status->setText(text);
        }
        if (m_state->phase==2) { m_cancel->setEnabled(false); m_cancel->setOpacity(100); }
        if (!finished) return;
        unschedule(schedule_selector(UpdatePopup::tick)); m_finished=true;
        if (!error.empty()) {
            setTitle("Update stopped"); m_status->setText(error);
            m_cancel->setEnabled(true); m_cancel->setOpacity(255);
            static_cast<ButtonSprite*>(m_cancel->getNormalImage())->setString("Close"); m_cancel->updateSprite(); return;
        }
        for (auto const& id:m_state->enable) if (auto* mod=Loader::get()->getInstalledMod(id)) {
            if (!mod->enable()) {
                setTitle("Mods installed"); m_status->setText("Enable the required mod "+id+" in Geode, then restart.");
                m_cancel->setEnabled(true); m_cancel->setOpacity(255);
                static_cast<ButtonSprite*>(m_cancel->getNormalImage())->setString("Close"); m_cancel->updateSprite();
                { std::lock_guard lock(m_state->mutex); m_state->error="Required mod could not be enabled."; }
                return;
            }
        }
        // Match Geode's restart path: let this message render before saving and relaunching.
        runAction(CCSequence::create(CCDelayTime::create(1.5f),CallFuncExt::create([] { game::restart(true); }),nullptr));
    }
    void keyBackClicked() override { if (m_finished && !m_state->error.empty()) onClose(nullptr); }
public:
    static UpdatePopup* create() { auto* p=new UpdatePopup(); if (p->init()) {p->autorelease();return p;} delete p;return nullptr; }
};
}
class $modify(DamianUpdateMenu,MenuLayer) {
    void onEnter() {
        MenuLayer::onEnter();
        if (getChildByIDRecursive("auto-update-button"_spr)) return;
        auto* menu=typeinfo_cast<CCMenu*>(getChildByID("bottom-menu"));
        if (!menu) return;
        auto* button=CCMenuItemSpriteExtra::create(ButtonSprite::create("Auto Update"),this,menu_selector(DamianUpdateMenu::onUpdate));
        button->setID("auto-update-button"_spr); button->setScale(.45f); menu->addChild(button); menu->updateLayout();
    }
    void onUpdate(CCObject*) {
        if (updater::busy()) return;
        if (auto* popup=UpdatePopup::create()) popup->show();
    }
};
