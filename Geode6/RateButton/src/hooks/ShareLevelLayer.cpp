#include <Geode/Geode.hpp>
#include <Geode/modify/ShareLevelLayer.hpp>
#include "../DiffPickPopup.hpp"
#include "../utils/Utils.hpp"

using namespace geode::prelude;

class $modify(MyShareLevelLayer, ShareLevelLayer) {
    struct Fields {
        CCMenuItemSpriteExtra* switchButton = nullptr;
        CCLabelBMFont* caption = nullptr;
        CCPoint stockPosition;
        bool positionSaved = false;
        int demonDiff = 4;
        int easyVariant = 0;
    };

    void renameRequestHeader() {
        auto walk = [](CCNode* node, auto&& self) -> void {
            if (!node) return;
            if (auto* label = typeinfo_cast<CCLabelBMFont*>(node)) {
                std::string text = label->getString();
                if (text == "REQUEST STARS" || text == "Request Stars" || text == "Request stars") {
                    label->setString("DIFFICULTY REQUESTED");
                    label->limitLabelWidth(280.f, label->getScale(), .35f);
                }
            }
            if (auto* children = node->getChildren())
                for (auto* child : CCArrayExt<CCNode*>(children)) self(child, self);
        };
        walk(this, walk);
    }

    void ensureVisuals() {
        if (!m_difficultySprite || !m_difficultySprite->getParent() || !m_buttonMenu) return;
        if (!m_fields->positionSaved) {
            m_fields->stockPosition = m_difficultySprite->getPosition();
            m_fields->positionSaved = true;
        }
        if (!m_fields->caption) {
            auto* label = CCLabelBMFont::create("", "bigFont.fnt");
            label->setID("diff-request-caption"_spr);
            label->setAlignment(kCCTextAlignmentCenter);
            m_difficultySprite->getParent()->addChild(label, m_difficultySprite->getZOrder() + 1);
            m_fields->caption = label;
        }
        if (!m_fields->switchButton) {
            auto* sprite = ButtonSprite::create("Switch", 56, true, "goldFont.fnt", "GJ_button_05.png", 24.f, .55f);
            auto* button = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(MyShareLevelLayer::onDiffSwitch));
            button->setScale(.8f);
            button->setID("diff-switch-button"_spr);
            m_buttonMenu->addChild(button);
            m_fields->switchButton = button;
        }
    }

    char const* stockFrame() {
        if (m_starsRequested == 10) {
            auto* frame = Utils::getDemonFaceFrame(m_fields->demonDiff);
            return frame ? frame : "difficulty_10_btn_001.png";
        }
        switch (m_starsRequested) {
            case 1: return "difficulty_auto_btn_001.png";
            case 2: return "difficulty_01_btn_001.png";
            case 3: return "difficulty_02_btn_001.png";
            case 4: case 5: return "difficulty_03_btn_001.png";
            case 6: case 7: return "difficulty_04_btn_001.png";
            case 8: case 9: return "difficulty_05_btn_001.png";
            default: return "difficulty_00_btn_001.png";
        }
    }

    void updateCustomFaceVisuals() {
        ensureVisuals();
        if (!m_difficultySprite || !m_fields->caption || !m_fields->switchButton) return;
        auto* host = m_difficultySprite;
        // Reset the stock frame and scale on every pass; repeated selections
        // cannot compound a custom texture's scale or leave an old caption.
        auto* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(stockFrame());
        if (frame) host->setDisplayFrame(frame);
        host->setScale(1.f);
        host->setPosition(m_fields->stockPosition + CCPoint{0.f, 12.f});
        host->setVisible(m_starsRequested > 0);
        host->setColor({255, 255, 255});
        auto const customDemon = m_starsRequested == 10 && Utils::isCustomDemonFace(m_fields->demonDiff);
        auto const customEasy = m_starsRequested == Utils::EASY_STARS && m_fields->easyVariant == 1;
        bool hasCustomFace = false;
        if (customDemon || customEasy) {
            auto* custom = CCSprite::create(customDemon ? "impossible_demon.png"_spr : "very_easy.png"_spr);
            if (custom && custom->getTexture()) {
                auto const size = custom->getContentSize();
                host->setTexture(custom->getTexture());
                host->setTextureRect({0.f, 0.f, size.width, size.height});
                host->setScale(36.f / std::max(size.width, size.height));
                hasCustomFace = true;
            }
        }
        float bottom = host->getPositionY() - host->getScaledContentSize().height * host->getAnchorPoint().y;
        auto* caption = m_fields->caption;
        caption->setVisible(hasCustomFace);
        if (hasCustomFace) {
            caption->setString(customDemon ? "IMPOSSIBLE\nDEMON" : "VERY\nEASY");
            caption->setScale(.24f);
            caption->limitLabelWidth(76.f, .24f, .16f);
            float const height = caption->getScaledContentSize().height;
            caption->setPosition({host->getPositionX(), bottom - 3.f - height * .5f});
            bottom -= height + 3.f;
        }
        // Ordinary difficulty textures already contain the full name. They
        // receive no second label; the switch sits below the actual caption.
        auto* button = m_fields->switchButton;
        button->setVisible(m_starsRequested == 10 || m_starsRequested == Utils::EASY_STARS);
        auto const world = host->getParent()->convertToWorldSpace({host->getPositionX(),
            bottom - 6.f - button->getScaledContentSize().height * .5f});
        button->setPosition(m_buttonMenu->convertToNodeSpace(world));
    }

    void onDiffSwitch(CCObject*) {
        std::vector<DiffPickOption> options;
        if (m_starsRequested == 10) {
            for (int i = 0; i < Utils::DEMON_SUBTYPE_COUNT; ++i) {
                DiffPickOption opt{Utils::getDemonDisplayLabel(i), i, {}, {}};
                if (auto* frame = Utils::getDemonFaceFrame(i)) opt.frame = frame;
                else opt.customSpr = "impossible_demon.png"_spr;
                options.push_back(std::move(opt));
            }
        } else if (m_starsRequested == Utils::EASY_STARS) {
            options = {{"Easy", 0, "difficulty_01_btn_001.png", {}}, {"Very Easy", 1, {}, "very_easy.png"_spr}};
        } else return;
        auto const family = m_starsRequested;
        if (auto* popup = DiffPickPopup::create(family == 10 ? "Select Demon" : "Select Difficulty", options,
            [this, family](int value) {
                if (family == 10) m_fields->demonDiff = value;
                else m_fields->easyVariant = value;
                updateCustomFaceVisuals();
            })) popup->show();
    }

    bool init(GJGameLevel* level) {
        if (!ShareLevelLayer::init(level)) return false;
        if (Utils::isOnGdps()) { renameRequestHeader(); updateCustomFaceVisuals(); }
        return true;
    }
    void selectRating(CCObject* sender) {
        ShareLevelLayer::selectRating(sender);
        if (!Utils::isOnGdps()) return;
        if (m_starsRequested != 10) m_fields->demonDiff = 4;
        if (m_starsRequested != Utils::EASY_STARS) m_fields->easyVariant = 0;
        renameRequestHeader();
        updateCustomFaceVisuals();
    }
    void setupStars() {
        ShareLevelLayer::setupStars();
        if (Utils::isOnGdps()) { renameRequestHeader(); updateCustomFaceVisuals(); }
    }
};
