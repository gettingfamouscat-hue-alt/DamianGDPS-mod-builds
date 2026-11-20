#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>

#include "../DiffPickPopup.hpp"
#include "../utils/CustomFace.hpp"
#include "../utils/SearchFilters.hpp"
#include "../utils/Utils.hpp"

#include <utility>
#include <vector>

using namespace geode::prelude;

class $modify(MyLevelSearchLayer, LevelSearchLayer) {
    struct Fields {
        CCMenuItemSpriteExtra *m_easyTypeButton = nullptr;
        CCSprite *m_easyFaceHost = nullptr;
    };

    CCMenu *difficultyMenu() {
        if (auto *menu = typeinfo_cast<CCMenu *>(this->getChildByID("difficulty-filter-menu")))
            return menu;
        return this->getChildByType<CCMenu>(3);
    }

    CCMenuItemSpriteExtra *easyFilterButton() {
        if (auto *menu = this->difficultyMenu()) {
            if (auto *btn = typeinfo_cast<CCMenuItemSpriteExtra *>(
                    menu->getChildByID("easy-filter-button")
                )) {
                return btn;
            }
            // Fallback: Easy is usually index 1 (after N/A).
            if (auto *children = menu->getChildren()) {
                if (children->count() > 1)
                    return typeinfo_cast<CCMenuItemSpriteExtra *>(children->objectAtIndex(1));
            }
        }
        return nullptr;
    }

    void ensureEasyPlusButton() {
        if (!Utils::isOnGdps() || m_fields->m_easyTypeButton)
            return;

        auto *menu = this->difficultyMenu();
        auto *easyBtn = this->easyFilterButton();
        auto *demonPlus = m_demonTypeButton;
        if (!menu || !easyBtn)
            return;

        CCSprite *plusSpr = nullptr;
        if (demonPlus && demonPlus->getNormalImage()) {
            if (auto *src = typeinfo_cast<CCSprite *>(demonPlus->getNormalImage())) {
                plusSpr = CCSprite::createWithSpriteFrame(src->displayFrame());
                plusSpr->setScale(src->getScale());
            }
        }
        if (!plusSpr) {
            plusSpr = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
            if (plusSpr)
                plusSpr->setScale(0.6f);
        }
        if (!plusSpr)
            return;

        m_fields->m_easyTypeButton = CCMenuItemSpriteExtra::create(
            plusSpr,
            this,
            menu_selector(MyLevelSearchLayer::onEasyTypeFilter)
        );
        m_fields->m_easyTypeButton->setID("easy-type-filter-button"_spr);
        m_fields->m_easyTypeButton->setVisible(false);

        menu->addChild(m_fields->m_easyTypeButton);
        this->layoutSubtypeButtons();
    }

    void layoutSubtypeButtons() {
        auto* menu = this->difficultyMenu();
        if (!menu) return;
        auto* demon = menu->getChildByID("demon-filter-button");
        auto* automatic = menu->getChildByID("auto-filter-button");
        if (!demon || !automatic) return;
        auto const position = CCPoint{
            demon->getPositionX() + (automatic->getPositionX() - demon->getPositionX()) * .43f,
            demon->getPositionY() + 12.f
        };
        for (auto* button : {m_demonTypeButton, m_fields->m_easyTypeButton}) {
            if (!button || !button->getParent()) continue;
            if (auto* image = button->getNormalImage()) {
                auto const width = image->getContentSize().width;
                if (width > 0.f) image->setScale(14.f / width);
            }
            button->setPosition(button->getParent()->convertToNodeSpace(menu->convertToWorldSpace(position)));
        }
    }

    bool easyDiffSelected() {
        // Diff index for Easy in LevelSearchLayer toggles is typically 1 (GJDifficulty::Easy).
        return this->checkDiff(static_cast<int>(GJDifficulty::Easy));
    }

    void updateEasyPlusVisibility() {
        this->ensureEasyPlusButton();
        if (!m_fields->m_easyTypeButton)
            return;
        this->layoutSubtypeButtons();
        auto const show = Utils::isOnGdps() && this->easyDiffSelected()
            && !this->checkDiff(static_cast<int>(GJDifficulty::Demon));
        m_fields->m_easyTypeButton->setVisible(show);
        if (!show && SearchFilters::easyFilter() != SearchFilters::EASY_FILTER_ALL) {
            // Keep saved preference; only clear visuals when Easy isn't selected.
        }
        this->updateEasyFaceVisual();
    }

    CCNode *easyFaceSprite() {
        auto *easyBtn = this->easyFilterButton();
        if (!easyBtn)
            return nullptr;
        return easyBtn->getNormalImage();
    }

    void updateEasyFaceVisual() {
        auto *host = this->easyFaceSprite();
        if (!host || !host->getParent())
            return;

        CustomFace::clearOverlays(host->getParent(), host);

        if (!Utils::isOnGdps() || !this->easyDiffSelected())
            return;

        if (SearchFilters::easyFilter() == SearchFilters::EASY_FILTER_VERY_EASY) {
            CustomFace::applyOverlay(
                host,
                "very_easy.png"_spr,
                CustomFace::ID_VERY_EASY,
                CustomFace::ID_VERY_EASY_LABEL,
                nullptr
            );
        }
    }

    void updateDemonFaceVisual() {
        if (!Utils::isOnGdps() || !SearchFilters::impossibleActive())
            return;

        // Demon filter button face â†’ Impossible overlay.
        CCNode *host = nullptr;
        if (auto *menu = this->difficultyMenu()) {
            if (auto *btn = typeinfo_cast<CCMenuItemSpriteExtra *>(
                    menu->getChildByID("demon-filter-button")
                )) {
                host = btn->getNormalImage();
            }
        }
        if (!host && m_lastDifficultySprite)
            host = m_lastDifficultySprite;
        if (!host || !host->getParent())
            return;

        CustomFace::applyOverlay(
            host,
            "impossible_demon.png"_spr,
            CustomFace::ID_IMPOSSIBLE,
            CustomFace::ID_IMPOSSIBLE_LABEL,
            nullptr
        );
    }

    void openEasyTypePopup() {
        std::vector<DiffPickOption> options{
            {
                "Easy",
                SearchFilters::EASY_FILTER_REGULAR,
                "difficulty_01_btn_001.png",
                {}
            },
            {
                "Very Easy",
                SearchFilters::EASY_FILTER_VERY_EASY,
                {},
                "very_easy.png"_spr
            },
        };

        if (auto *popup = DiffPickPopup::create("Select Difficulty", options, [this](int value) {
                SearchFilters::setEasyFilter(value);
                this->updateEasyFaceVisual();
            })) {
            popup->show();
        }
    }

    void onEasyTypeFilter(CCObject *) {
        this->openEasyTypePopup();
    }

    bool init(int type) {
        if (!LevelSearchLayer::init(type))
            return false;

        if (!Utils::isOnGdps())
            return true;

        this->ensureEasyPlusButton();
        this->updateEasyPlusVisibility();
        if (SearchFilters::impossibleActive())
            this->updateDemonFaceVisual();
        return true;
    }

    void demonFilterSelectClosed(int filter) {
        if (Utils::isOnGdps() && filter == SearchFilters::IMPOSSIBLE_FILTER_TAG) {
            SearchFilters::setImpossible(true);
            // Select Demon difficulty in the filter row, then show Impossible face.
            // Stock closed handler expects 0/6/7/8/9/10 â€” feed Extreme so Demon stays on,
            // then overlay Impossible and keep our search flag.
            LevelSearchLayer::demonFilterSelectClosed(
                static_cast<int>(GJDifficulty::DemonExtreme)
            );
            SearchFilters::setImpossible(true);
            this->updateDemonFaceVisual();
            return;
        }

        if (Utils::isOnGdps())
            SearchFilters::setImpossible(false);

        LevelSearchLayer::demonFilterSelectClosed(filter);

        if (Utils::isOnGdps()) {
            // Clear any leftover Impossible overlay when picking stock subtypes.
            if (auto *menu = this->difficultyMenu()) {
                if (auto *btn = typeinfo_cast<CCMenuItemSpriteExtra *>(
                        menu->getChildByID("demon-filter-button")
                    )) {
                    if (auto *host = btn->getNormalImage())
                        CustomFace::clearOverlays(host->getParent(), host);
                }
            }
        }
    }

    void toggleDifficulty(CCObject *sender) {
        LevelSearchLayer::toggleDifficulty(sender);
        if (!Utils::isOnGdps())
            return;

        if (!this->easyDiffSelected()) {
            // Leaving Easy clears the Easy subtype preference visually; keep saved
            // value so re-selecting Easy restores the last + choice.
            if (auto *host = this->easyFaceSprite()) {
                if (host->getParent())
                    CustomFace::clearOverlays(host->getParent(), host);
            }
        } else {
            this->updateEasyFaceVisual();
        }

        // Leaving Demon clears Impossible search flag + overlay.
        if (!this->checkDiff(static_cast<int>(GJDifficulty::Demon))) {
            if (SearchFilters::impossibleActive()) {
                SearchFilters::setImpossible(false);
                if (auto *menu = this->difficultyMenu()) {
                    if (auto *btn = typeinfo_cast<CCMenuItemSpriteExtra *>(
                            menu->getChildByID("demon-filter-button")
                        )) {
                        if (auto *host = btn->getNormalImage())
                            CustomFace::clearOverlays(host->getParent(), host);
                    }
                }
            }
        }

        this->updateEasyPlusVisibility();
    }

    void clearFilters() {
        LevelSearchLayer::clearFilters();
        if (!Utils::isOnGdps())
            return;
        SearchFilters::clearAll();
        this->updateEasyPlusVisibility();
        if (auto *host = this->easyFaceSprite()) {
            if (host->getParent())
                CustomFace::clearOverlays(host->getParent(), host);
        }
    }
};
