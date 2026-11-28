#include <Geode/Geode.hpp>
#include <Geode/modify/ShareLevelSettingsLayer.hpp>
#include <Geode/ui/TextInput.hpp>
#include <algorithm>
#include "../utils/Utils.hpp"

using namespace geode::prelude;

class $modify(GdpsCopySettings, ShareLevelSettingsLayer) {
    struct Fields {
        CCMenuItemToggler* allowCopy = nullptr;
        CCMenuItemToggler* requirePassword = nullptr;
        TextInput* passwordInput = nullptr;
    };

    void refreshCopySettings() {
        bool allowed = m_fields->allowCopy && m_fields->allowCopy->isToggled();
        bool protectedCopy = allowed && m_fields->requirePassword && m_fields->requirePassword->isToggled();
        if (m_fields->passwordInput) {
            m_fields->passwordInput->setVisible(protectedCopy);
            m_fields->passwordInput->setEnabled(protectedCopy);
        }
    }

    void onCopyToggle(CCObject* sender) {
        // CCMenuItemToggler invokes its callback before flipping its state.
        if (sender == m_fields->requirePassword && !m_fields->requirePassword->isToggled())
            m_fields->allowCopy->toggle(true);
        if (sender == m_fields->allowCopy && m_fields->allowCopy->isToggled())
            m_fields->requirePassword->toggle(false);
        this->scheduleOnce(schedule_selector(GdpsCopySettings::refreshAfterToggle), 0.f);
    }

    void refreshAfterToggle(float) {
        refreshCopySettings();
    }

    void hideVanillaCopyNotice(CCNode* node) {
        if (!node) return;
        if (auto* label = typeinfo_cast<CCLabelBMFont*>(node)) {
            if (std::string(label->getString()).find("Starting in 2.2") != std::string::npos) label->setVisible(false);
        }
        if (auto* label = typeinfo_cast<CCLabelTTF*>(node)) {
            if (std::string(label->getString()).find("Starting in 2.2") != std::string::npos) label->setVisible(false);
        }
        if (auto* children = node->getChildren()) {
            for (auto* child : CCArrayExt<CCNode*>(children)) hideVanillaCopyNotice(child);
        }
    }

    bool saveCopySettings() {
        if (!Utils::isOnGdps() || !m_level || !m_fields->allowCopy) return true;
        if (!m_fields->allowCopy->isToggled()) {
            m_level->setPassword(0);
            return true;
        }
        if (!m_fields->requirePassword->isToggled()) {
            m_level->setPassword(1);
            return true;
        }
        std::string code = m_fields->passwordInput->getString();
        if ((code.size() != 4 && code.size() != 6) || code[0] == '0' ||
            !std::all_of(code.begin(), code.end(), [](unsigned char c) { return c >= '0' && c <= '9'; })) {
            FLAlertLayer::create("Copy Password", "Enter 4 or 6 digits, starting with 1-9.", "OK")->show();
            return false;
        }
        m_level->setPassword(1000000 + std::stoi(code));
        return true;
    }

    bool init(GJGameLevel* level, ShareLevelLayer* parent) {
        if (!ShareLevelSettingsLayer::init(level, parent)) return false;
        if (!Utils::isOnGdps() || !level || !m_mainLayer || !m_buttonMenu) return true;

        auto size = m_mainLayer->getContentSize();
        if (size.width < 100.f || size.height < 100.f) size = CCDirector::sharedDirector()->getWinSize();
        float const x = size.width * .5f;
        float const y = size.height * .5f;
        hideVanillaCopyNotice(m_mainLayer);

        auto menuPosition = [this](CCPoint point) {
            return m_buttonMenu->convertToNodeSpace(m_mainLayer->convertToWorldSpace(point));
        };

        auto* allow = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(GdpsCopySettings::onCopyToggle), .65f);
        allow->setID("allow-copy"_spr);
        allow->setPosition(menuPosition({x - 103.f, y + 12.f}));
        m_buttonMenu->addChild(allow);
        auto* allowLabel = CCLabelBMFont::create("Allow Copy", "bigFont.fnt");
        allowLabel->setScale(.36f);
        allowLabel->setAnchorPoint({0.f, .5f});
        allowLabel->setPosition({x - 84.f, y + 12.f});
        m_mainLayer->addChild(allowLabel);

        auto* require = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(GdpsCopySettings::onCopyToggle), .65f);
        require->setID("require-copy-password"_spr);
        require->setPosition(menuPosition({x - 103.f, y - 12.f}));
        m_buttonMenu->addChild(require);
        auto* requireLabel = CCLabelBMFont::create("Require Password", "bigFont.fnt");
        requireLabel->setScale(.36f);
        requireLabel->setAnchorPoint({0.f, .5f});
        requireLabel->setPosition({x - 84.f, y - 12.f});
        m_mainLayer->addChild(requireLabel);

        auto* input = TextInput::create(120.f, "4 or 6 digits", "bigFont.fnt");
        input->setCommonFilter(CommonFilter::Uint);
        input->setMaxCharCount(6);
        input->setScale(.8f);
        input->setPosition({x, y - 43.f});
        input->setID("copy-password-input"_spr);
        m_mainLayer->addChild(input);

        int pass = level->m_password;
        allow->toggle(pass != 0);
        require->toggle(pass > 1);
        if (pass > 1) {
            int code = pass >= 1000000 ? pass - 1000000 : pass;
            input->setString(std::to_string(code));
        }
        m_fields->allowCopy = allow;
        m_fields->requirePassword = require;
        m_fields->passwordInput = input;
        refreshCopySettings();
#ifdef GEODE_IS_MACOS
        // Route the native close button through the supported back handler.
        auto* close = typeinfo_cast<CCMenuItemSpriteExtra*>(getChildByIDRecursive("close-button"));
        if (!close && m_buttonMenu) close = m_buttonMenu->getChildByType<CCMenuItemSpriteExtra>(0);
        if (close) close->setTarget(this, menu_selector(GdpsCopySettings::onCopySettingsClose));
#endif
        return true;
    }

#ifdef GEODE_IS_MACOS
    void onCopySettingsClose(CCObject*) {
        if (!saveCopySettings()) return;
        ShareLevelSettingsLayer::keyBackClicked();
    }
#else
    void onClose(CCObject* sender) {
        if (!saveCopySettings()) return;
        ShareLevelSettingsLayer::onClose(sender);
    }
#endif

    void keyBackClicked() {
        if (!saveCopySettings()) return;
        ShareLevelSettingsLayer::keyBackClicked();
    }
};
