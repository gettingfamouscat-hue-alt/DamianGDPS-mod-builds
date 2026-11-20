#include <Geode/Geode.hpp>
#include <Geode/modify/DemonFilterSelectLayer.hpp>
#include "../utils/SearchFilters.hpp"
#include "../utils/Utils.hpp"
#include <array>

using namespace geode::prelude;

class $modify(MyDemonFilterSelectLayer, DemonFilterSelectLayer) {
    void addImpossibleButton() {
        if (!Utils::isOnGdps() || !m_mainLayer) return;
        auto* filters = typeinfo_cast<CCMenu*>(m_mainLayer->getChildByID("demon-filters"));
        auto* bg = m_mainLayer->getChildByID("background");
        if (!filters || !bg || filters->getChildByID("impossible-demon-filter-button"_spr)) return;
        constexpr std::array<char const*, 6> ids{
            "all-demon-filter-button", "easy-demon-filter-button", "medium-demon-filter-button",
            "hard-demon-filter-button", "insane-demon-filter-button", "extreme-demon-filter-button"
        };
        std::vector<CCMenuItemSpriteExtra*> buttons;
        for (auto id : ids) {
            auto* button = typeinfo_cast<CCMenuItemSpriteExtra*>(filters->getChildByID(id));
            if (!button) return;
            buttons.push_back(button);
        }
        auto const rowY = buttons.front()->getPositionY();
        auto const size = bg->getContentSize();
        auto const left = filters->convertToNodeSpace(bg->convertToWorldSpace({20.f, size.height * .5f})).x;
        auto const right = filters->convertToNodeSpace(bg->convertToWorldSpace({size.width - 20.f, size.height * .5f})).x;
        float const slot = (right - left) / 7.f;
        if (slot <= 0.f) return;

        auto* option = CCSprite::create();
        option->setContentSize({40.f, 48.f});
        option->setCascadeColorEnabled(true);
        option->setCascadeOpacityEnabled(true);
        auto* face = CCSprite::create("impossible_demon.png"_spr);
        if (!face || !face->getTexture()) return;
        face->setScale(32.f / std::max(face->getContentSize().width, face->getContentSize().height));
        face->setPosition({20.f, 31.f});
        option->addChild(face);
        auto* caption = CCLabelBMFont::create("IMPOSSIBLE\nDEMON", "bigFont.fnt");
        caption->setAlignment(kCCTextAlignmentCenter);
        caption->limitLabelWidth(44.f, .20f, .12f);
        caption->setPosition({20.f, 6.f});
        option->addChild(caption);
        auto* button = CCMenuItemSpriteExtra::create(option, this,
            menu_selector(DemonFilterSelectLayer::selectRating));
        button->setTag(SearchFilters::IMPOSSIBLE_FILTER_TAG);
        button->setID("impossible-demon-filter-button"_spr);
        filters->addChild(button);
        buttons.push_back(button);
        // Fit seven complete choices within the original background.
        for (size_t i = 0; i < buttons.size(); ++i) {
            auto* item = buttons[i];
            item->setScale(std::min(1.f, (slot - 4.f) / item->getContentSize().width));
            item->setPosition({left + slot * (static_cast<float>(i) + .5f), rowY});
        }
    }

    bool init() {
        if (!DemonFilterSelectLayer::init()) return false;
        addImpossibleButton();
        return true;
    }

    void selectRating(CCObject* sender) {
        if (Utils::isOnGdps() && static_cast<CCNode*>(sender)->getTag() == SearchFilters::IMPOSSIBLE_FILTER_TAG) {
            SearchFilters::setImpossible(true);
            if (m_delegate) m_delegate->demonFilterSelectClosed(SearchFilters::IMPOSSIBLE_FILTER_TAG);
            onClose(nullptr);
            return;
        }
        if (Utils::isOnGdps()) SearchFilters::setImpossible(false);
        DemonFilterSelectLayer::selectRating(sender);
    }
};
