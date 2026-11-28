#include "DevRateStarsPopup.hpp"
#include "CustomStarsPopup.hpp"
#include "DiffPickPopup.hpp"
#include "managers/SessionManager.hpp"
#include "managers/SettingsManager.hpp"
#include "managers/StaffRatingClient.hpp"
#include "utils/CustomFace.hpp"
#include "utils/Utils.hpp"

#include <chrono>
#include <thread>
#include <utility>
#include <vector>

using namespace geode::prelude;

bool DevRateStarsPopup::init(int levelId) {
    if (!SessionManager::canUseRateButton() || !Popup::init(430.f, 300.f)) return false;
    m_levelId = levelId;
    bool elder = SessionManager::isElderMod();
    setTitle(elder ? "Dev: Set Stars" : "Advisor: Suggest Stars", "bigFont.fnt", .75f);
    setID("dev-rate-stars-popup"_spr);

    auto addText = [this](char const* text, CCPoint position, float scale) {
        auto* label = CCLabelBMFont::create(text, "bigFont.fnt");
        label->setScale(scale); label->setPosition(position); m_mainLayer->addChild(label); return label;
    };
    for (int n = elder ? 0 : 1; n <= Utils::GRANDPA_LAST_STARS; ++n) {
        auto* sprite = ButtonSprite::create(std::to_string(n).c_str(), 34, true,
            "bigFont.fnt", "GJ_button_04.png", 32.f, .6f);
        auto* button = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(DevRateStarsPopup::onNumber));
        button->setTag(n); button->setID(fmt::format("stars-pick-{}"_spr, n));
        if (n == 0) button->setPosition({395.f, 266.f});
        else if (n <= 10) button->setPosition({55.f + ((n-1)%5)*47.f, n <= 5 ? 235.f : 192.f});
        else { button->setScale(.85f); button->setPosition({41.f + (n-11)*43.f, 151.f}); }
        m_buttonMenu->addChild(button); m_numberButtons.push_back(button);
    }
    addText("Extra demons", {147.f, 130.f}, .24f);
    for (int n = 0; n < Utils::DEMON_SUBTYPE_COUNT; ++n) {
        auto* face = CCSprite::createWithSpriteFrameName(Utils::getDemonFaceFrame(n));
        face->setScale(.75f);
        auto* button = CCMenuItemSpriteExtra::create(face, this, menu_selector(DevRateStarsPopup::onDemon));
        button->setTag(n); button->setPosition({55.f + n*47.f, 105.f});
        m_buttonMenu->addChild(button); m_demonButtons.push_back(button);
    }
    m_starsLabel = addText("0", {350.f, 171.f}, .55f);
    m_difficultyFace = CCSprite::createWithSpriteFrameName("difficulty_00_btn_001.png");
    m_difficultyFaceButton = CCMenuItemSpriteExtra::create(m_difficultyFace, this, menu_selector(DevRateStarsPopup::onDifficultyFace));
    m_difficultyFaceButton->setPosition({350.f, 217.f}); m_buttonMenu->addChild(m_difficultyFaceButton);
    m_switchButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Switch", 56, true, "goldFont.fnt", "GJ_button_05.png", 24.f, .55f),
        this, menu_selector(DevRateStarsPopup::onSwitch));
    m_switchButton->setPosition({350.f, 145.f}); m_buttonMenu->addChild(m_switchButton);
    m_grandpaLabel = addText("", {350.f, 145.f}, .3f);

    m_starsToggleSprite = CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
    m_starsToggleButton = CCMenuItemSpriteExtra::create(m_starsToggleSprite, this, menu_selector(DevRateStarsPopup::onStarsAwardToggle));
    m_starsToggleButton->setPosition({27.f, 267.f}); m_buttonMenu->addChild(m_starsToggleButton);
    m_starsOffMark = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
    m_starsOffMark->setScale(.55f); m_starsToggleButton->addChild(m_starsOffMark);
    m_starsOffMark->setAnchorPoint({.5f, .5f});
    m_starsOffMark->setPosition(m_starsToggleButton->convertToNodeSpace(
        m_starsToggleSprite->convertToWorldSpace(m_starsToggleSprite->getContentSize()/2.f)));
    m_awardLabel = addText("Given: 0", {142.f, 76.f}, .35f);
    if (elder) {
        auto* custom = CCMenuItemSpriteExtra::create(ButtonSprite::create("Set Stars"), this, menu_selector(DevRateStarsPopup::onCustomStars));
        custom->setScale(.5f); custom->setPosition({142.f, 53.f}); m_buttonMenu->addChild(custom);
    }
    auto* coin = CCSprite::createWithSpriteFrameName("secretCoinUI2_001.png"); coin->setScale(.5f);
    m_coinsButton = CCMenuItemSpriteExtra::create(coin, this, menu_selector(DevRateStarsPopup::onCoins));
    m_coinsButton->setPosition({elder ? 350.f : 390.f, 102.f}); m_buttonMenu->addChild(m_coinsButton);
    addText("Silver coins", {elder ? 350.f : 380.f, 76.f}, .23f);
    if (!elder) {
        auto* fire = CCSprite::createWithSpriteFrameName("GJ_featuredCoin_001.png"); fire->setScale(.65f);
        m_featureButton = CCMenuItemSpriteExtra::create(fire, this, menu_selector(DevRateStarsPopup::onFeature));
        m_featureButton->setPosition({315.f, 102.f}); m_featureButton->setOpacity(DISABLED_OPACITY);
        m_buttonMenu->addChild(m_featureButton); addText("Fire", {315.f, 76.f}, .23f);
    }
    auto* cancel = CCMenuItemSpriteExtra::create(ButtonSprite::create("Cancel"), this, menu_selector(DevRateStarsPopup::onCancel));
    auto* submit = CCMenuItemSpriteExtra::create(ButtonSprite::create("Submit"), this, menu_selector(DevRateStarsPopup::onSubmit));
    cancel->setScale(.7f); submit->setScale(.7f);
    cancel->setPosition({155.f, 25.f}); submit->setPosition({275.f, 25.f});
    m_buttonMenu->addChild(cancel); m_buttonMenu->addChild(submit); m_submitButton = submit;
    updateStarsToggleVisuals(); updateDifficultyVisuals();
    m_coinsButton->setOpacity(DISABLED_OPACITY);
    return true;
}

void DevRateStarsPopup::onNumber(CCObject* sender) {
    m_selectedStars = static_cast<CCNode*>(sender)->getTag();
    updateDifficultyVisuals();
}
void DevRateStarsPopup::onDemon(CCObject* sender) {
    m_demonDiff = static_cast<CCNode*>(sender)->getTag();
    updateDifficultyVisuals();
}

int DevRateStarsPopup::starsToSubmit() const {
    return m_awardStars ? (m_customStars >= 0 ? m_customStars : m_selectedStars) : 0;
}

void DevRateStarsPopup::updateAwardLabel() {
    if (m_awardLabel)
        m_awardLabel->setString(fmt::format("Given: {}", starsToSubmit()).c_str());
}

void DevRateStarsPopup::onCustomStars(CCObject *) {
    if (!SessionManager::isElderMod()) return;
    int current = m_customStars >= 0 ? m_customStars : m_selectedStars;
    auto weak = WeakRef(this);
    if (auto* popup = CustomStarsPopup::create(current, [weak](int stars) {
            if (auto self = weak.lock()) { self->m_customStars = stars; self->updateAwardLabel(); }
        })) popup->show();
}

void DevRateStarsPopup::updateStarsToggleVisuals() {
    if (m_starsToggleButton) {
        m_starsToggleButton->setOpacity(m_awardStars ? ENABLED_OPACITY : DISABLED_OPACITY);
    }
    if (m_starsToggleSprite) {
        m_starsToggleSprite->setColor(m_awardStars ? ccColor3B{ 255, 255, 255 } : ccColor3B{ 140, 140, 140 });
    }
    if (m_starsOffMark) {
        m_starsOffMark->setVisible(!m_awardStars);
    }
    // Dim the count when it won't be awarded, but keep picker usable for face
    if (m_starsLabel) {
        m_starsLabel->setColor(m_awardStars ? ccColor3B{ 255, 255, 255 } : ccColor3B{ 160, 160, 160 });
    }
}

void DevRateStarsPopup::updateSwitchVisibility() {
    if (!m_switchButton)
        return;
    auto const show = m_selectedStars == Utils::EASY_STARS;
    m_switchButton->setVisible(show);
    m_switchButton->setEnabled(show);
    if (m_grandpaLabel) {
        bool const extra = m_selectedStars >= Utils::GRANDPA_FIRST_STARS
            && m_selectedStars <= Utils::GRANDPA_LAST_STARS;
        m_grandpaLabel->setVisible(extra);
        if (extra) {
            m_grandpaLabel->setString(Utils::grandpaLabels[m_selectedStars - Utils::GRANDPA_FIRST_STARS]);
            m_grandpaLabel->limitLabelWidth(95.f, .32f, .18f);
        }
    }
}

void DevRateStarsPopup::onStarsAwardToggle(CCObject *) {
    m_awardStars = !m_awardStars;
    this->updateStarsToggleVisuals();
    this->updateDifficultyVisuals();
}

void DevRateStarsPopup::onFeature(CCObject *) {
    auto *sprite = static_cast<CCSprite *>(m_featureButton->getNormalImage());
    auto *spriteCache = CCSpriteFrameCache::sharedSpriteFrameCache();

    m_featureState = static_cast<FeatureState>((static_cast<int>(m_featureState) + 1) % 5);

    char const *frame = "GJ_featuredCoin_001.png";
    switch (m_featureState) {
        case FeatureState::Epic:
            frame = "GJ_epicCoin_001.png";
            break;
        case FeatureState::Legendary:
            frame = "GJ_epicCoin2_001.png";
            break;
        case FeatureState::Mythic:
            frame = "GJ_epicCoin3_001.png";
            break;
        default:
            break;
    }

    sprite->setDisplayFrame(spriteCache->spriteFrameByName(frame));
    m_featureButton->setOpacity(m_featureState == FeatureState::None ? DISABLED_OPACITY : ENABLED_OPACITY);
}

void DevRateStarsPopup::onCoins(CCObject *) {
    m_coinsEnabled = !m_coinsEnabled;
    m_coinsButton->setOpacity(m_coinsEnabled ? ENABLED_OPACITY : DISABLED_OPACITY);
}

void DevRateStarsPopup::onDecrease(CCObject *) {
    if (m_selectedStars <= 0)
        return;

    m_selectedStars--;
    this->updateDifficultyVisuals();
}

void DevRateStarsPopup::onIncrease(CCObject *) {
    if (m_selectedStars >= Utils::GRANDPA_LAST_STARS)
        return;

    m_selectedStars++;
    this->updateDifficultyVisuals();
}

void DevRateStarsPopup::onDifficultyFace(CCObject *) {
    // Secondary shortcut: cycle demon subtypes at 10 stars
    if (m_selectedStars != 10)
        return;

    m_demonDiff = (m_demonDiff + 1) % Utils::DEMON_SUBTYPE_COUNT;
    this->updateDifficultyVisuals();
}

void DevRateStarsPopup::onSwitch(CCObject *) {
    if (m_selectedStars == 10) {
        this->openDemonSwitchPopup();
        return;
    }
    if (m_selectedStars == Utils::EASY_STARS) {
        this->openEasySwitchPopup();
    }
}

void DevRateStarsPopup::openDemonSwitchPopup() {
    std::vector<DiffPickOption> options;
    for (int i = 0; i < Utils::DEMON_SUBTYPE_COUNT; ++i) {
        DiffPickOption opt;
        opt.label = Utils::getDemonDisplayLabel(i);
        opt.value = i;
        if (auto const *frame = Utils::getDemonFaceFrame(i))
            opt.frame = frame;
        else
            opt.customSpr = "impossible_demon.png"_spr;
        options.push_back(std::move(opt));
    }

    if (auto *popup = DiffPickPopup::create("Select Demon", options, [this](int value) {
            m_demonDiff = value;
            this->updateDifficultyVisuals();
        })) {
        popup->show();
    }
}

void DevRateStarsPopup::openEasySwitchPopup() {
    std::vector<DiffPickOption> options{
        { "Easy", 0, "difficulty_01_btn_001.png", {} },
        { "Very Easy", 1, {}, "very_easy.png"_spr },
    };

    if (auto *popup = DiffPickPopup::create("Select Difficulty", options, [this](int value) {
            m_easyVariant = value;
            this->updateDifficultyVisuals();
        })) {
        popup->show();
    }
}

void DevRateStarsPopup::onDaily(CCObject *) {
    GameLevelManager::sharedState()->uploadComment(
        "!daily", CommentType::Level, m_levelId, 0
    );

    this->onClose(nullptr);
}

void DevRateStarsPopup::onWeekly(CCObject *) {
    GameLevelManager::sharedState()->uploadComment(
        "!weekly", CommentType::Level, m_levelId, 0
    );

    this->onClose(nullptr);
}

void DevRateStarsPopup::refreshLevelPage() const {
    auto const levelId = m_levelId;
    std::thread([levelId]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(900));
        Loader::get()->queueInMainThread([levelId]() {
            GameLevelManager::sharedState()->downloadLevel(levelId, false, 0);

            auto *scene = CCDirector::sharedDirector()->getRunningScene();
            if (!scene)
                return;

            if (auto *lil = scene->getChildByType<LevelInfoLayer>(0)) {
                if (lil->m_level && static_cast<int>(lil->m_level->m_levelID) == levelId) {
                    lil->downloadLevel();
                }
            }
        });
    }).detach();
}

void DevRateStarsPopup::onSendOnly(CCObject*) { submitRating(false); }

void DevRateStarsPopup::submitRating(bool rate) {
    if (m_pending) return;
    m_pending = true; m_submitButton->setEnabled(false); m_submitButton->setOpacity(127);
    auto weak = WeakRef(this);
    StaffRatingClient::request(m_levelId, rate ? "stars" : "send", {
        {"difficulty", std::string(difficultyCommandName())}, {"stars", std::to_string(starsToSubmit())},
        {"coins", m_coinsEnabled ? "1" : "0"}, {"tier", std::to_string(static_cast<int>(m_featureState))}
    }, [weak, level = m_levelId, rate](bool ok, std::string message, matjson::Value) {
        auto popup = weak.lock(); if (!popup) return;
        popup->m_pending = false; popup->m_submitButton->setEnabled(true); popup->m_submitButton->setOpacity(255);
        if (!ok) { FLAlertLayer::create("Could not save", message, "OK")->show(); return; }
        Notification::create(rate ? "Rating saved" : "Sent to the Sent tab", NotificationIcon::Success)->show();
        popup->onClose(nullptr); StaffRatingClient::refreshLevel(level);
    });
}

void DevRateStarsPopup::onCancel(CCObject *) {
    this->onClose(nullptr);
}

std::string_view DevRateStarsPopup::difficultyCommandName() const {
    if (m_selectedStars >= Utils::GRANDPA_FIRST_STARS
        && m_selectedStars <= Utils::GRANDPA_LAST_STARS)
        return Utils::grandpaCommands[m_selectedStars - Utils::GRANDPA_FIRST_STARS];
    if (m_selectedStars == 10)
        return Utils::getDemonDifficultyName(m_demonDiff);
    if (m_selectedStars == Utils::EASY_STARS && m_easyVariant == 1)
        return "very-easy";
    return Utils::getLevelDifficulty(m_selectedStars);
}

void DevRateStarsPopup::onSubmit(CCObject*) {
    submitRating(SessionManager::isElderMod());
}

bool DevRateStarsPopup::applyCustomFaceTexture(CCSprite *face, char const *sprName) {
    return CustomFace::applyTextureKeepingSize(face, sprName);
}

void DevRateStarsPopup::updateDifficultyVisuals() {
    auto *spriteCache = CCSpriteFrameCache::sharedSpriteFrameCache();
    updateAwardLabel();
    for (auto* button : m_numberButtons) {
        auto* sprite = static_cast<ButtonSprite*>(button->getNormalImage());
        sprite->setColor(button->getTag() == m_selectedStars ? ccColor3B{110, 255, 70} : ccColor3B{255, 255, 255});
    }
    for (auto* button : m_demonButtons) {
        button->setVisible(m_selectedStars == 10);
        button->setOpacity(button->getTag() == m_demonDiff ? 255 : 140);
    }

    static constexpr std::array difficultyFrames {
        "difficulty_00_btn_001.png", // N/A
        "difficulty_auto_btn_001.png", // Auto
        "difficulty_01_btn_001.png", // Easy
        "difficulty_02_btn_001.png", // Normal
        "difficulty_03_btn_001.png", // Hard
        "difficulty_03_btn_001.png", // Hard
        "difficulty_04_btn_001.png", // Harder
        "difficulty_04_btn_001.png", // Harder
        "difficulty_05_btn_001.png", // Insane
        "difficulty_05_btn_001.png", // Insane
        "difficulty_06_btn_001.png" // Demon (overridden below by subtype)
    };

    // Show picker value; if toggle off, append hint that submit sends 0
    if (m_awardStars) {
        m_starsLabel->setString(std::to_string(m_selectedStars).c_str());
        m_starsLabel->setScale(1.0f);
    } else {
        m_starsLabel->setString(fmt::format("{}→0", m_selectedStars).c_str());
        m_starsLabel->setScale(m_selectedStars >= 10 ? 0.55f : 0.7f);
    }

    this->updateSwitchVisibility();

    auto *face = static_cast<CCSprite *>(m_difficultyFaceButton->getNormalImage());
    if (!face)
        return;

    if (m_selectedStars >= Utils::GRANDPA_FIRST_STARS
        && m_selectedStars <= Utils::GRANDPA_LAST_STARS) {
        int const index = m_selectedStars - Utils::GRANDPA_FIRST_STARS;
        if (auto* frame = spriteCache->spriteFrameByName(Utils::grandpaFrames[index])) {
            face->setDisplayFrame(frame);
            face->setScale(1.25f);
            return;
        }
        // The extra demon art is supplied by the Grandpa Demon mod.
        face->setDisplayFrame(spriteCache->spriteFrameByName("difficulty_10_btn_001.png"));
        face->setScale(1.25f);
        return;
    }

    if (m_selectedStars == 10 && Utils::isCustomDemonFace(m_demonDiff)) {
        if (this->applyCustomFaceTexture(face, "impossible_demon.png"_spr))
            return;
        // Fall back to Extreme if the resource failed to load
        face->setDisplayFrame(spriteCache->spriteFrameByName("difficulty_10_btn_001.png"));
        face->setScale(1.25f);
        return;
    }

    if (m_selectedStars == Utils::EASY_STARS && m_easyVariant == 1) {
        if (this->applyCustomFaceTexture(face, "very_easy.png"_spr))
            return;
        face->setDisplayFrame(spriteCache->spriteFrameByName("difficulty_01_btn_001.png"));
        face->setScale(1.25f);
        return;
    }

    char const *frame = nullptr;
    if (m_selectedStars == 10) {
        frame = Utils::getDemonFaceFrame(m_demonDiff);
    } else {
        frame = (m_selectedStars >= 0 && m_selectedStars < static_cast<int>(difficultyFrames.size()))
            ? difficultyFrames[m_selectedStars] : "difficulty_00_btn_001.png";
    }

    face->setDisplayFrame(spriteCache->spriteFrameByName(frame));
    face->setScale(1.25f);
}

DevRateStarsPopup *DevRateStarsPopup::create(int levelId) {
    auto ret = new DevRateStarsPopup();
    if (ret->init(levelId)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}
