#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include "../DevRateStarsPopup.hpp"
#include "../FeaturedOrderPopup.hpp"
#include "../AdminDemonPopup.hpp"
#include "../CreatorDisplayPopup.hpp"
#include "../managers/SessionManager.hpp"
#include "../utils/CustomFace.hpp"
#include "../utils/Utils.hpp"
#include "../utils/PercentDisplay.hpp"

#include <chrono>
#include <string>
#include <thread>

using namespace geode::prelude;

namespace {

void rewriteStarsRequestedText(CCNode *node, char const *diffName) {
    if (!node)
        return;

    if (auto *label = typeinfo_cast<CCLabelBMFont *>(node)) {
        std::string text = label->getString();
        if (text.find("Stars Requested") != std::string::npos
            || text.find("Stars requested") != std::string::npos) {
            label->setString(fmt::format("Difficulty Requested: {}", diffName).c_str());
        }
    }

    if (auto *area = typeinfo_cast<TextArea *>(node)) {
        // TextArea stores the source string; rebuild if it mentions stars requested.
        // Walk children for the actual BMFont lines as a reliable fallback.
    }

    if (auto *children = node->getChildren()) {
        for (auto *child : CCArrayExt<CCNode *>(children))
            rewriteStarsRequestedText(child, diffName);
    }
}

} // namespace

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        CCSprite *m_customFaceOverlay = nullptr;
        CreatorDisplay::State display;
        bool starVisible = false, demonVisible = false;
        bool staffPositionSaved = false;
        CCPoint staffPosition{};
    };

    void hideVanillaRateButtons() {
        if (m_starRateBtn)
            m_starRateBtn->setVisible(!SessionManager::isElderMod() && m_fields->starVisible);
        if (m_demonRateBtn)
            m_demonRateBtn->setVisible(false);

        // Stock mod suggest/rate uses the same yellow GJ_starBtnMod sprite as us.
        // Hide that duplicate whenever our Rate Button is in play (elders especially).
        auto hideModRate = [](CCNode *menu) {
            if (!menu)
                return;
            if (auto *btn = menu->getChildByID("mod-rate-button"))
                btn->setVisible(false);
            for (auto *child : CCArrayExt<CCNode *>(menu->getChildren())) {
                if (child->getID() == "mod-rate-button")
                    child->setVisible(false);
            }
        };

        hideModRate(this->getChildByID("left-side-menu"));
        hideModRate(this->getChildByID("right-side-menu"));
    }

    CCMenu* rateMenu() {
        if (auto* menu = typeinfo_cast<CCMenu*>(getChildByID("staff-controls-menu"_spr))) return menu;
        auto* menu = CCMenu::create(); menu->setPosition({0.f, 0.f});
        menu->setID("staff-controls-menu"_spr); addChild(menu, 10);
        return menu;
    }

    void clearCustomFaceOverlay() {
        if (m_difficultySprite)
            CustomFace::clearOverlays(m_difficultySprite->getParent(), m_difficultySprite);
        m_fields->m_customFaceOverlay = nullptr;
    }

    void applyCustomDifficultyFace() {
        this->clearCustomFaceOverlay();

        // Faces are not gated on isOnGdps — Mac URL offsets can fail while the
        // GDPS still serves Very Easy / Impossible demonDiff flags.
        if (!m_level || !m_difficultySprite)
            return;

        if (CustomFace::applyForLevel(m_difficultySprite, m_level)) {
            m_fields->m_customFaceOverlay = typeinfo_cast<CCSprite *>(
                m_difficultySprite->getParent()->getChildByID(CustomFace::ID_VERY_EASY)
            );
            if (!m_fields->m_customFaceOverlay) {
                m_fields->m_customFaceOverlay = typeinfo_cast<CCSprite *>(
                    m_difficultySprite->getParent()->getChildByID(CustomFace::ID_IMPOSSIBLE)
                );
            }
        }
        CustomFace::setStarRowNudge(this, Utils::isVeryEasyLevel(m_level));
    }

    void ensureRateButton() {
        SessionManager::ensureLoaded();
        this->hideVanillaRateButtons();

        auto *menu = this->rateMenu();
        if (!menu) {
            log::warn("Rate Button: side menu not found");
            return;
        }

        auto win = CCDirector::sharedDirector()->getWinSize();
        if (!m_fields->staffPositionSaved) {
            m_fields->staffPosition = CCPoint{30.f, win.height / 2.f + 30.f};
            for (auto const* id : {"left-side-menu", "right-side-menu"})
                if (auto* nativeMenu = getChildByID(id))
                    if (auto* nativeButton = nativeMenu->getChildByID("mod-rate-button"))
                        m_fields->staffPosition = menu->convertToNodeSpace(nativeButton->convertToWorldSpace(nativeButton->getContentSize()/2.f));
            m_fields->staffPositionSaved = true;
        }
        auto* adminDemon = menu->getChildByID("admin-set-demon-button"_spr);
        if (SessionManager::canSetDemon() && !adminDemon) {
            auto* button = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_demonIcon_001.png"), this,
                menu_selector(MyLevelInfoLayer::onSetDemon));
            button->setID("admin-set-demon-button"_spr); button->setPosition({win.width - 82.f, 30.f}); menu->addChild(button);
        } else if (!SessionManager::canSetDemon() && adminDemon) adminDemon->removeFromParent();
        auto* adminStars = menu->getChildByID("admin-set-stars-button"_spr);
        if (SessionManager::isElderMod() && !adminStars) {
            auto* button = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_starBtn_001.png"), this,
                menu_selector(MyLevelInfoLayer::onSetStars));
            button->setID("admin-set-stars-button"_spr); button->setPosition({win.width - 30.f, 30.f}); menu->addChild(button);
        } else if (!SessionManager::isElderMod() && adminStars) {
            adminStars->removeFromParent(); 
        }
        auto *existing = menu->getChildByID("dev-rate-button"_spr);

        if (!SessionManager::canUseRateButton()) {
            if (existing) {
                existing->removeFromParent();
                
            }
            return;
        }

        // Always hide stock mod-rate when our button is available.
        this->hideVanillaRateButtons();

        if (existing)
            return;

        auto *devRateButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_starBtnMod_001.png"),
            this,
            menu_selector(MyLevelInfoLayer::onDevRateButton)
        );
        devRateButton->setID("dev-rate-button"_spr);

        devRateButton->setPosition(m_fields->staffPosition);
        menu->addChild(devRateButton);
        
        log::info("Rate Button: added yellow mod star (access={})", SessionManager::ModAccessLevel);
    }

    bool init(GJGameLevel *level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge))
            return false;

        m_fields->starVisible = m_starRateBtn && m_starRateBtn->isVisible();
        m_fields->demonVisible = m_demonRateBtn && m_demonRateBtn->isVisible();
        SessionManager::ensureLoaded();
        // Custom faces always — Very Easy / Impossible encodings are GDPS-only.
        this->applyCustomDifficultyFace();

        if (!Utils::isOnGdps())
            return true;

        if (m_level && PercentDisplay::frozen(static_cast<int>(m_level->m_normalPercent)))
            PercentDisplay::capExactLabels(this, this->getChildByIDRecursive("level-name"));

        SessionManager::refresh(true);
        this->ensureRateButton();
        this->updateCreatorDisplay();
        this->schedule(schedule_selector(MyLevelInfoLayer::pollPermissions), .5f);

        return true;
    }

    void onCreatorDisplay(CCObject*) {
        if(SessionManager::canManageRoles() && m_level)
            if(auto* p=CreatorDisplayPopup::create(m_level->m_levelID)) p->show();
    }
    void updateCreatorDisplay() {
        if(!m_level || m_level->m_levelID<=0) return;
        CreatorDisplay::apply(this,m_level->m_levelID,m_fields->display,false);
        auto* menu=this->rateMenu(); if(!menu)return;
        auto* existing=menu->getChildByID("creator-display-button"_spr);
        if(!SessionManager::canManageRoles()) { if(existing){existing->removeFromParent();}return; }
        if(existing)return;
        auto* sprite=ButtonSprite::create("Creator");
        sprite->setScale(.45f);
        auto* b=CCMenuItemSpriteExtra::create(sprite,this,menu_selector(MyLevelInfoLayer::onCreatorDisplay));
        b->setID("creator-display-button"_spr); b->setPosition(m_fields->staffPosition + CCPoint{0.f, 38.f}); menu->addChild(b);
    }
    void pollPermissions(float) {
        SessionManager::refresh();
        this->ensureRateButton();
        this->updateCreatorDisplay();
    }

    void onEnter() {
        LevelInfoLayer::onEnter();
        if(Utils::isOnGdps()) this->schedule(schedule_selector(MyLevelInfoLayer::pollPermissions),.5f);
    }
    void onExit() {
        this->unschedule(schedule_selector(MyLevelInfoLayer::pollPermissions));
        LevelInfoLayer::onExit();
    }

    void updateLabelValues() {
        LevelInfoLayer::updateLabelValues();
        this->applyCustomDifficultyFace();
    }

    void levelDownloadFinished(GJGameLevel *level) {
        LevelInfoLayer::levelDownloadFinished(level);
        this->applyCustomDifficultyFace();
        if (!Utils::isOnGdps())
            return;
        this->ensureRateButton();
        this->updateCreatorDisplay();
        this->updateLabelValues();
    }

    void levelUpdateFinished(GJGameLevel *level, UpdateResponse response) {
        LevelInfoLayer::levelUpdateFinished(level, response);
        this->applyCustomDifficultyFace();
        if (!Utils::isOnGdps())
            return;
        this->ensureRateButton();
        this->updateCreatorDisplay();
        this->updateLabelValues();
    }

    void onLevelInfo(CCObject *sender) {
        LevelInfoLayer::onLevelInfo(sender);
        if (!Utils::isOnGdps() || !m_level)
            return;

        auto const *name = Utils::getRequestedDifficultyLabel(
            static_cast<int>(m_level->m_starsRequested)
        );

        // onLevelInfo shows an FLAlertLayer; rewrite "Stars Requested" lines.
        if (auto *scene = CCDirector::sharedDirector()->getRunningScene()) {
            for (auto *child : CCArrayExt<CCNode *>(scene->getChildren())) {
                if (auto *alert = typeinfo_cast<FLAlertLayer *>(child))
                    rewriteStarsRequestedText(alert, name);
            }
        }
        // Also check children of this layer (some builds parent the alert here).
        rewriteStarsRequestedText(this, name);
    }

    void onSetDemon(CCObject*) {
        if (m_level && SessionManager::canSetDemon())
            if (auto* popup = AdminDemonPopup::create(m_level->m_levelID)) popup->show();
    }
    void onSetStars(CCObject*) {
        if (m_level && SessionManager::isElderMod())
            if (auto* popup = DevRateStarsPopup::create(m_level->m_levelID)) popup->show();
    }
    void onDevRateButton(CCObject*) {
        if (!m_level || !SessionManager::canUseRateButton()) return;
        if (SessionManager::isElderMod()) {
            if (auto* popup = FeaturedOrderPopup::create(m_level->m_levelID)) popup->show();
        } else if (auto* popup = DevRateStarsPopup::create(m_level->m_levelID)) popup->show();
    }
};
