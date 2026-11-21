#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include "../DevRateStarsPopup.hpp"
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
    };

    void hideVanillaRateButtons() {
        if (m_starRateBtn)
            m_starRateBtn->setVisible(false);
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

    CCMenu *rateMenu() {
        if (auto *left = static_cast<CCMenu *>(this->getChildByID("left-side-menu")))
            return left;
        if (auto *right = static_cast<CCMenu *>(this->getChildByID("right-side-menu")))
            return right;
        return nullptr;
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

        auto *existing = menu->getChildByID("dev-rate-button"_spr);

        if (!SessionManager::canUseRateButton()) {
            if (existing) {
                existing->removeFromParent();
                menu->updateLayout();
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

        menu->addChild(devRateButton);
        menu->updateLayout();
        log::info("Rate Button: added yellow mod star (access={})", SessionManager::ModAccessLevel);
    }

    bool init(GJGameLevel *level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge))
            return false;

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
        if(!SessionManager::canManageRoles()) { if(existing){existing->removeFromParent();menu->updateLayout();}return; }
        if(existing)return;
        auto* sprite=ButtonSprite::create("Creator");
        sprite->setScale(.45f);
        auto* b=CCMenuItemSpriteExtra::create(sprite,this,menu_selector(MyLevelInfoLayer::onCreatorDisplay));
        b->setID("creator-display-button"_spr);menu->addChild(b);menu->updateLayout();
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

    void onDevRateButton(CCObject *) {
        if (!SessionManager::canUseRateButton()) {
            FLAlertLayer::create(
                "No Permission",
                "You don't have rating permissions on this GDPS.",
                "OK"
            )->show();
            return;
        }

        if (!SessionManager::ShownRateButtonWarning) {
            geode::createQuickPopup(
                "Rate Button Warning",
                "<cy>Elder Mod</c> can <cg>rate</c> levels.\n"
                "<cy>Normal Mod</c> (Send Mod) can only <co>send</c> levels to the Sent tab.\n"
                "If Damian hasn't given you a mod role, this button won't appear.\n\n"
                "<cy>Do you acknowledge this?</c>\n<co>This will not be shown again.</c>",
                "No", "Yes",
                [levelID = static_cast<int>(m_level->m_levelID)](auto, bool btn2) {
                    if (btn2) {
                        Mod::get()->setSavedValue("shown-rate-button-warning", true);
                        SessionManager::ShownRateButtonWarning = true;

                        if (auto* popup = DevRateStarsPopup::create(levelID)) popup->show();
                    }
                }
            );
        } else {
            if (auto* popup = DevRateStarsPopup::create(m_level->m_levelID)) popup->show();
        }
    }
};
