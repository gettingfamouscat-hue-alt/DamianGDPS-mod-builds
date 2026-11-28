#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include "../DevRateStarsPopup.hpp"
#include "../FeaturedOrderPopup.hpp"
#include "../CreatorDisplayPopup.hpp"
#include "../managers/SessionManager.hpp"
#include "../utils/CustomFace.hpp"
#include "../utils/Utils.hpp"
#include "../utils/PercentDisplay.hpp"

#include <chrono>
#include <string>
#include <thread>
#include <algorithm>
#include <vector>

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
        Ref<CCMenu> staffMenu;
        std::vector<std::pair<Ref<CCNode>, CCPoint>> innerButtons;
        bool staffPositionSaved = false;
        CCPoint staffPosition{};
        CCPoint creatorPosition{};
    };

    void hideVanillaRateButtons() {
        if (m_starRateBtn) m_starRateBtn->setVisible(!SessionManager::isElderMod() && m_fields->starVisible);
        if (m_demonRateBtn) m_demonRateBtn->setVisible(false);
        if (auto* demon = getChildByIDRecursive("demon-rate-button")) demon->setVisible(false);
    }

    CCMenu* rateMenu() {
        if (m_fields->staffMenu && m_fields->staffMenu->getParent() == this) return m_fields->staffMenu;
        auto* menu = CCMenu::create(); menu->setPosition({0.f, 0.f});
        menu->setID("staff-controls-menu"_spr); addChild(menu, 10); m_fields->staffMenu = menu;
        return menu;
    }

    bool containsFrame(CCNode* node, char const* name) {
        if (!node) return false;
        if (auto* sprite = typeinfo_cast<CCSprite*>(node))
            if (auto* frame = sprite->displayFrame())
                if (std::string(frame->getFrameName()) == name) return true;
        for (auto* child : CCArrayExt<CCNode*>(node->getChildren()))
            if (containsFrame(child, name)) return true;
        return false;
    }
    CCMenuItemSpriteExtra* nativeModButton(CCNode* node) {
        if (node == m_fields->staffMenu.data()) return nullptr;
        if (auto* button = typeinfo_cast<CCMenuItemSpriteExtra*>(node))
            if (node->getID() == "mod-rate-button" || containsFrame(button->getNormalImage(), "GJ_starBtnMod_001.png")) return button;
        for (auto* child : CCArrayExt<CCNode*>(node->getChildren()))
            if (auto* found = nativeModButton(child)) return found;
        return nullptr;
    }
    void reserveBottomRow(bool active) {
        if (m_fields->innerButtons.empty()) {
            if (auto* other = getChildByID("other-menu"))
                for (auto const* id : {"move-up-button", "move-down-button", "folder-button", "list-button"})
                    if (auto* button = other->getChildByID(id)) m_fields->innerButtons.push_back({button, button->getPosition()});
        }
        auto win = CCDirector::sharedDirector()->getWinSize();
        for (auto const& [button, original] : m_fields->innerButtons) {
            if (!button->getParent()) continue;
            if (!active) { button->setPosition(original); continue; }
            auto world = button->getParent()->convertToWorldSpace(original);
            world.x = win.width - 120.f;
            button->setPosition(button->getParent()->convertToNodeSpace(world));
        }
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
        SessionManager::ensureLoaded(); hideVanillaRateButtons();
        auto* menu = rateMenu(); auto win = CCDirector::sharedDirector()->getWinSize();
        bool admin = SessionManager::isElderMod();
        if (!m_fields->staffPositionSaved) {
            m_fields->staffPosition = CCPoint{30.f, win.height / 2.f + 30.f};
            m_fields->creatorPosition = m_fields->staffPosition + CCPoint{0.f, 38.f};
            m_fields->staffPositionSaved = true;
        }
        if (auto* left = getChildByID("left-side-menu")) {
            if (auto* remove = left->getChildByID("delete-server-button")) {
                m_fields->creatorPosition = menu->convertToNodeSpace(left->convertToWorldSpace(remove->getPosition()));
                Ref<CCNode> retained = remove; remove->removeFromParentAndCleanup(false); menu->addChild(retained);
            }
        }
        if (auto* remove = menu->getChildByID("delete-server-button")) {
            remove->setVisible(admin); remove->setAnchorPoint({.5f,.5f});
            remove->setScale(40.f / std::max(1.f, remove->getContentSize().width));
            remove->setPosition({win.width - 80.f, 30.f});
        }
        reserveBottomRow(admin);
        if (auto* old = menu->getChildByID("admin-set-demon-button"_spr)) old->removeFromParent();
        auto* stars = menu->getChildByID("admin-set-stars-button"_spr);
        if (admin && !stars) {
            auto* button = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_starBtn_001.png"), this, menu_selector(MyLevelInfoLayer::onSetStars));
            button->setID("admin-set-stars-button"_spr);
            button->setScale(40.f / std::max(1.f, button->getContentSize().width));
            button->setPosition({win.width - 30.f, 30.f}); menu->addChild(button);
        } else if (!admin && stars) stars->removeFromParent();

        auto* custom = menu->getChildByID("dev-rate-button"_spr);
        if (auto* native = nativeModButton(this)) {
            if (custom) custom->removeFromParent();
            native->setVisible(SessionManager::canUseRateButton());
            native->setEnabled(SessionManager::canUseRateButton());
            native->setTarget(this, menu_selector(MyLevelInfoLayer::onDevRateButton));
            return;
        }
        if (!SessionManager::canUseRateButton()) { if (custom) custom->removeFromParent(); return; }
        if (!custom) {
            auto* button = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_starBtnMod_001.png"), this, menu_selector(MyLevelInfoLayer::onDevRateButton));
            button->setID("dev-rate-button"_spr); button->setScale(.65f);
            button->setPosition(m_fields->staffPosition); menu->addChild(button);
        }
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
        // Staff nodes are added onEnter, after Node IDs has finished init.
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
        sprite->setScale(.35f);
        auto* b=CCMenuItemSpriteExtra::create(sprite,this,menu_selector(MyLevelInfoLayer::onCreatorDisplay));
        b->setID("creator-display-button"_spr); b->setPosition(m_fields->creatorPosition); menu->addChild(b);
    }
    void pollPermissions(float) {
        SessionManager::refresh();
        this->ensureRateButton();
        this->updateCreatorDisplay();
    }

    void onEnter() {
        LevelInfoLayer::onEnter();
        if(Utils::isOnGdps()) {
            ensureRateButton(); updateCreatorDisplay();
            this->schedule(schedule_selector(MyLevelInfoLayer::pollPermissions),.5f);
        }
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
