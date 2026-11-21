#include "DevRateStarsPopup.hpp"
#include "CustomStarsPopup.hpp"
#include "DiffPickPopup.hpp"
#include "managers/SessionManager.hpp"
#include "managers/SettingsManager.hpp"
#include "utils/CustomFace.hpp"
#include "utils/Utils.hpp"

#include <chrono>
#include <thread>
#include <utility>
#include <vector>

using namespace geode::prelude;

bool DevRateStarsPopup::init(int levelId) {
    if (!SessionManager::canUseRateButton()) return false;
    if (!Popup::init(360.f, 210.f))
        return false;

    m_levelId = levelId;

    // Elder = full rate UI. Access 1 (Mod) = send-to-mod queue only.
    auto const elder = SessionManager::isElderMod();
    this->setTitle(elder ? "Mod: Rate Level" : "Mod: Send Mod", "bigFont.fnt", 1.0f);
    m_title->setPositionY(m_title->getPositionY() - 5.f);

    this->setID("dev-rate-stars-popup"_spr);

    /*
        STAR AWARD TOGGLE
        Logo on  = submit selected star count
        Logo off = submit 0 stars (difficulty face / fire / coins still apply)
    */
    m_starsToggleSprite = CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
    m_starsToggleSprite->setScale(1.15f);

    m_starsToggleButton = CCMenuItemSpriteExtra::create(
        m_starsToggleSprite,
        this,
        menu_selector(DevRateStarsPopup::onStarsAwardToggle)
    );
    m_starsToggleButton->setID("stars-award-toggle"_spr);
    // Sit above the star count — keep clear of the popup close (X) button
    m_buttonMenu->addChildAtPosition(m_starsToggleButton, Anchor::Left, { 130.f, 42.f });

    // Red X overlay when stars are not awarded
    m_starsOffMark = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
    m_starsOffMark->setScale(0.55f);
    m_starsOffMark->setVisible(false);
    m_starsOffMark->setID("stars-award-off-mark"_spr);
    m_starsToggleButton->addChild(m_starsOffMark);
    m_starsOffMark->setPosition(m_starsToggleSprite->getContentSize() / 2.f);

    /*
        FEATURE BUTTON LOGIC
    */
    m_featureButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_featuredCoin_001.png"),
        this,
        menu_selector(DevRateStarsPopup::onFeature)
    );
    m_featureButton->setOpacity(DISABLED_OPACITY);
    m_featureButton->setID("feature-button"_spr);
    m_buttonMenu->addChildAtPosition(m_featureButton, Anchor::TopRight, { -10.f, -10.f });

    /*
        COINS BUTTON LOGIC
    */
    auto *coinsSprite = CCSprite::createWithSpriteFrameName("secretCoinUI2_001.png");
    coinsSprite->setScale(44.f / 53.75f);

    m_coinsButton = CCMenuItemSpriteExtra::create(
        coinsSprite,
        this,
        menu_selector(DevRateStarsPopup::onCoins)
    );
    m_coinsButton->setOpacity(DISABLED_OPACITY);
    m_coinsButton->setID("coins-button"_spr);
    m_buttonMenu->addChildAtPosition(m_coinsButton, Anchor::BottomRight, { -10.f, 10.f });

    /*
        LOWER STARS BUTTON LOGIC
    */
    m_decreaseStarsButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png"),
        this,
        menu_selector(DevRateStarsPopup::onDecrease)
    );
    m_decreaseStarsButton->setRotation(-90.f);
    m_decreaseStarsButton->setID("decrease-stars-button"_spr);
    m_buttonMenu->addChildAtPosition(m_decreaseStarsButton, Anchor::Left, { 85.f, 0.f });

    /*
        STARS LABEL LOGIC
    */
    m_starsLabel = CCLabelBMFont::create("0", "bigFont.fnt");
    m_starsLabel->setID("stars-label"_spr);
    m_buttonMenu->addChildAtPosition(m_starsLabel, Anchor::Left, { 130.f, 0.f });

    /*
        INCREASE STARS BUTTON LOGIC
    */
    m_increaseStarsButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png"),
        this,
        menu_selector(DevRateStarsPopup::onIncrease)
    );
    m_increaseStarsButton->setRotation(90.f);
    m_increaseStarsButton->setID("increase-stars-button"_spr);
    m_buttonMenu->addChildAtPosition(m_increaseStarsButton, Anchor::Left, { 175.f, 0.f });

    // Award amount can differ from the face chosen with the arrows.
    m_awardLabel = CCLabelBMFont::create("Given: 0", "bigFont.fnt");
    m_awardLabel->setScale(.35f);
    m_awardLabel->setID("stars-given-label"_spr);
    m_buttonMenu->addChildAtPosition(m_awardLabel, Anchor::Left, { 130.f, -27.f });
    if (elder) {
        auto* customButton = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Set Stars"), this,
            menu_selector(DevRateStarsPopup::onCustomStars));
        customButton->setScale(.53f);
        customButton->setID("custom-stars-button"_spr);
        m_buttonMenu->addChildAtPosition(customButton, Anchor::Left, { 130.f, -49.f });
    }

    /*
        DIFFICULTY FACE LOGIC
        Always driven by selected stars. At 10★ / 2★, Switch picks subtype.
        Face click still cycles demons at 10★ as a secondary shortcut.
    */
    m_difficultyFace = CCSprite::createWithSpriteFrameName("difficulty_00_btn_001.png");
    m_difficultyFace->setScale(1.25f);
    m_difficultyFace->setID("difficulty-face"_spr);

    m_difficultyFaceButton = CCMenuItemSpriteExtra::create(
        m_difficultyFace,
        this,
        menu_selector(DevRateStarsPopup::onDifficultyFace)
    );
    m_difficultyFaceButton->setID("difficulty-face-button"_spr);
    m_buttonMenu->addChildAtPosition(m_difficultyFaceButton, Anchor::Right, { -85.f, 12.f });

    auto *switchSprite = ButtonSprite::create(
        "Switch", 56, true, "goldFont.fnt", "GJ_button_05.png", 24.f, 0.55f
    );
    m_switchButton = CCMenuItemSpriteExtra::create(
        switchSprite,
        this,
        menu_selector(DevRateStarsPopup::onSwitch)
    );
    m_switchButton->setID("diff-switch-button"_spr);
    m_switchButton->setVisible(false);
    m_buttonMenu->addChildAtPosition(m_switchButton, Anchor::Right, { -85.f, -48.f });

    /*
        DAILY / WEEKLY — Elder only
    */
    if (elder && SettingsManager::ShowDailyButton) {
        auto *dailySprite = ButtonSprite::create(
            "Set\nDaily", 50, true, "goldFont.fnt", "GJ_button_01.png", 40.f, 1.0f
        );
        dailySprite->setScale(0.6f);

        auto *dailyButton = CCMenuItemSpriteExtra::create(
            dailySprite,
            this,
            menu_selector(DevRateStarsPopup::onDaily)
        );
        dailyButton->setID("daily-button"_spr);
        m_buttonMenu->addChildAtPosition(dailyButton, Anchor::Left, { 30.f, 15.f });
    }

    if (elder && SettingsManager::ShowWeeklyButton) {
        auto *weeklySprite = ButtonSprite::create(
            "Set\nWeek", 50, true, "goldFont.fnt", "GJ_button_01.png", 40.f, 1.0f
        );
        weeklySprite->setScale(0.6f);

        auto *weeklyButton = CCMenuItemSpriteExtra::create(
            weeklySprite,
            this,
            menu_selector(DevRateStarsPopup::onWeekly)
        );
        weeklyButton->setID("weekly-button"_spr);
        m_buttonMenu->addChildAtPosition(weeklyButton, Anchor::Left, { 30.f, -15.f });
    }

    /*
        SEND ONLY — always for normal mods; optional extra for elders
    */
    if (!elder || SettingsManager::ShowSendOnlyButton) {
        auto *sendOnlySprite = ButtonSprite::create(
            elder ? "Send\nOnly" : "Send", 50, true, "goldFont.fnt", "GJ_button_01.png", 40.f, 1.0f
        );
        sendOnlySprite->setScale(0.6f);

        auto *sendOnlyButton = CCMenuItemSpriteExtra::create(
            sendOnlySprite,
            this,
            menu_selector(DevRateStarsPopup::onSendOnly)
        );
        sendOnlyButton->setID("send-only-button"_spr);
        if (elder) {
            m_buttonMenu->addChildAtPosition(sendOnlyButton, Anchor::Right, { -30.f, -30.f });
        } else {
            m_buttonMenu->addChildAtPosition(sendOnlyButton, Anchor::Bottom, { 60.f, 25.f });
            sendOnlySprite->setScale(1.0f);
        }
    }

    /*
        CANCEL BUTTON LOGIC
    */
    auto *cancelButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Cancel", "goldFont.fnt", "GJ_button_01.png"),
        this,
        menu_selector(DevRateStarsPopup::onCancel)
    );
    cancelButton->setID("cancel-button"_spr);
    m_buttonMenu->addChildAtPosition(cancelButton, Anchor::Bottom, { -60.f, 25.f });

    /*
        SUBMIT (actual rate) — Elder only
    */
    if (elder) {
        auto *submitButton = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Submit", "goldFont.fnt", "GJ_button_01.png"),
            this,
            menu_selector(DevRateStarsPopup::onSubmit)
        );
        submitButton->setID("submit-button"_spr);
        m_buttonMenu->addChildAtPosition(submitButton, Anchor::Bottom, { 60.f, 25.f });
    }

    this->updateStarsToggleVisuals();
    this->updateDifficultyVisuals();

    return true;
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
    if (auto* popup = CustomStarsPopup::create(current, [this](int stars) {
            m_customStars = stars;
            updateAwardLabel();
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
    auto const show = (m_selectedStars == 10) || (m_selectedStars == Utils::EASY_STARS);
    m_switchButton->setVisible(show);
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
    if (m_selectedStars >= 10)
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

void DevRateStarsPopup::onSendOnly(CCObject *) {
    if (!SessionManager::canUseRateButton()) { Notification::create("No staff permission", NotificationIcon::Error)->show(); return; }
    auto const stars = this->starsToSubmit();

    if (m_selectedStars > 0) {
        // Trailing nonce so GD client / servers don't treat identical !send
        // as a duplicate comment and silently drop re-sends.
        auto const nonce = static_cast<long long>(
            std::chrono::steady_clock::now().time_since_epoch().count() % 1000000LL
        );
        GameLevelManager::sharedState()->uploadComment(
            fmt::format(
                "!send {} {} {} {} #{}",
                difficultyCommandName(),
                stars,
                static_cast<int>(m_featureState),
                m_coinsEnabled ? 1 : 0,
                nonce
            ),
            CommentType::Level, m_levelId, 0
        );
    } else {
        GameLevelManager::sharedState()->uploadComment(
            fmt::format(
                "!unsend #{}",
                static_cast<long long>(
                    std::chrono::steady_clock::now().time_since_epoch().count() % 1000000LL
                )
            ),
            CommentType::Level, m_levelId, 0
        );
    }

    this->refreshLevelPage();
    this->onClose(nullptr);
}

void DevRateStarsPopup::onCancel(CCObject *) {
    this->onClose(nullptr);
}

std::string_view DevRateStarsPopup::difficultyCommandName() const {
    if (m_selectedStars == 10)
        return Utils::getDemonDifficultyName(m_demonDiff);
    if (m_selectedStars == Utils::EASY_STARS && m_easyVariant == 1)
        return "very-easy";
    return Utils::getLevelDifficulty(m_selectedStars);
}

void DevRateStarsPopup::onSubmit(CCObject *) {
    if (!SessionManager::isElderMod()) { Notification::create("Rating requires Elder Mod or Admin", NotificationIcon::Error)->show(); return; }
    if (m_selectedStars > 0) {
        // Face always from picker. Stars only if the star logo toggle is on.
        GameLevelManager::sharedState()->uploadComment(
            fmt::format(
                "!rate {} {} {} {}",
                difficultyCommandName(),
                this->starsToSubmit(),
                m_coinsEnabled ? 1 : 0,
                static_cast<int>(m_featureState)
            ),
            CommentType::Level, m_levelId, 0
        );
    } else {
        GameLevelManager::sharedState()->uploadComment(
            "!unrate", CommentType::Level, m_levelId, 0
        );
    }

    this->refreshLevelPage();
    this->onClose(nullptr);
}

bool DevRateStarsPopup::applyCustomFaceTexture(CCSprite *face, char const *sprName) {
    return CustomFace::applyTextureKeepingSize(face, sprName);
}

void DevRateStarsPopup::updateDifficultyVisuals() {
    auto *spriteCache = CCSpriteFrameCache::sharedSpriteFrameCache();
    updateAwardLabel();

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
