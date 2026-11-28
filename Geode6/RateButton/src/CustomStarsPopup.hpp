#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/TextInput.hpp>
#include <charconv>
#include <functional>
#include <string>

using namespace geode::prelude;

class CustomStarsPopup : public Popup {
    TextInput* input = nullptr;
    std::function<void(int)> onChoice;

    bool init(int current, std::function<void(int)> callback, bool featured) {
        if (!Popup::init(270.f, 160.f)) return false;
        onChoice = std::move(callback);
        setTitle(featured ? "Featured Order" : "Stars Given");

        auto* hint = CCLabelBMFont::create(featured ? "0 to 2,147,483,647" : "0 to 2,147,483,647 stars", "bigFont.fnt");
        hint->setScale(.3f);
        hint->setPosition({135.f, 115.f});
        m_mainLayer->addChild(hint);
        auto* faceHint = CCLabelBMFont::create(featured ? "Lower appears first; 0 removes" : "Face stays the same", "bigFont.fnt");
        faceHint->setScale(.28f);
        faceHint->setPosition({135.f, 96.f});
        m_mainLayer->addChild(faceHint);

        input = TextInput::create(135.f, "Star amount");
        input->setFilter("0123456789");
        input->setString(std::to_string(current));
        input->setPosition({135.f, 70.f});
        m_mainLayer->addChild(input);

        auto* match = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Match Face"), this,
            menu_selector(CustomStarsPopup::onMatchFace));
        match->setScale(.65f);
        match->setPosition({73.f, 32.f});
        m_buttonMenu->addChild(match);
        match->setVisible(!featured);

        auto* save = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Save"), this,
            menu_selector(CustomStarsPopup::onSave));
        save->setScale(.7f);
        save->setPosition({195.f, 32.f});
        m_buttonMenu->addChild(save);
        return true;
    }

    void onMatchFace(CCObject*) {
        onChoice(-1);
        onClose(nullptr);
    }

    void onSave(CCObject*) {
        auto value = std::string(input->getString());
        int stars = 0;
        auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), stars);
        if (value.empty() || error != std::errc{} || end != value.data() + value.size()
            || stars < 0 || stars > 2147483647) {
            FLAlertLayer::create("Invalid stars", "Enter a whole number from 0 to 2,147,483,647.", "OK")->show();
            return;
        }
        onChoice(stars);
        onClose(nullptr);
    }

public:
    static CustomStarsPopup* create(int current, std::function<void(int)> callback, bool featured = false) {
        auto* popup = new CustomStarsPopup();
        if (popup->init(current, std::move(callback), featured)) {
            popup->autorelease();
            return popup;
        }
        delete popup;
        return nullptr;
    }
};
