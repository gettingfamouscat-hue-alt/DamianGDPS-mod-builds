#pragma once

using namespace geode::prelude;

class DevRateStarsPopup : public geode::Popup {
protected:
    enum class FeatureState {
        None = 0,
        Featured = 1,
        Epic = 2,
        Legendary = 3,
        Mythic = 4
    };
    static constexpr GLubyte DISABLED_OPACITY = 127; // 50%
    static constexpr GLubyte ENABLED_OPACITY = 255; // 100%

    int m_levelId = 0;
    CCMenuItemSpriteExtra *m_featureButton = nullptr;
    FeatureState m_featureState = FeatureState::None;
    CCMenuItemSpriteExtra *m_coinsButton = nullptr;
    bool m_coinsEnabled = false;
    CCMenuItemSpriteExtra *m_difficultyFaceButton = nullptr;
    CCSprite *m_difficultyFace = nullptr;
    CCLabelBMFont *m_starsLabel = nullptr;
    CCLabelBMFont *m_awardLabel = nullptr;
    CCLabelBMFont *m_grandpaLabel = nullptr;
    CCMenuItemSpriteExtra *m_decreaseStarsButton = nullptr;
    CCMenuItemSpriteExtra *m_increaseStarsButton = nullptr;
    // Star award toggle (logo). On = submit selected stars; Off = 0 stars (face/fire/coins still apply).
    CCMenuItemSpriteExtra *m_starsToggleButton = nullptr;
    CCSprite *m_starsToggleSprite = nullptr;
    CCSprite *m_starsOffMark = nullptr;
    // Switch under face → demon / Easy↔Very Easy pickers
    CCMenuItemSpriteExtra *m_switchButton = nullptr;
    int m_selectedStars = 0;
    int m_customStars = -1; // -1 follows the selected face; 0..INT32_MAX overrides only the award.
    // 0=Easy, 1=Medium, 2=Hard, 3=Insane, 4=Extreme, 5=Impossible (only used at 10 stars)
    int m_demonDiff = 2;
    // 0=Easy, 1=Very Easy (only used at 2 stars)
    int m_easyVariant = 0;
    // true = award stars on submit; false = face/fire/coins only (0 stars)
    bool m_awardStars = true;

    bool init(int levelId);

    void onFeature(CCObject *);
    void onCoins(CCObject *);
    void onDecrease(CCObject *);
    void onIncrease(CCObject *);
    void onDaily(CCObject *);
    void onWeekly(CCObject *);
    void onSendOnly(CCObject *);
    void onCancel(CCObject *);
    void onSubmit(CCObject *);
    void onDifficultyFace(CCObject *);
    void onStarsAwardToggle(CCObject *);
    void onCustomStars(CCObject *);
    void onSwitch(CCObject *);

    void updateDifficultyVisuals();
    void updateStarsToggleVisuals();
    void updateAwardLabel();
    void updateSwitchVisibility();
    bool applyCustomFaceTexture(CCSprite *face, char const *sprName);
    int starsToSubmit() const;
    std::string_view difficultyCommandName() const;
    void refreshLevelPage() const;

    void openDemonSwitchPopup();
    void openEasySwitchPopup();

public:
    static DevRateStarsPopup *create(int levelId);
};
