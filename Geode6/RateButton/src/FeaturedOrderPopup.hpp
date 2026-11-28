#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;
class FeaturedOrderPopup : public Popup {
    int m_level = 0, m_order = 0, m_tier = 0;
    bool m_ready = false, m_pending = false;
    CCLabelBMFont *m_number = nullptr, *m_fireText = nullptr;
    CCSprite* m_fire = nullptr;
    CCMenuItemSpriteExtra* m_submit = nullptr;
    bool init(int level);
    void update();
    void onStep(CCObject*);
    void onNumber(CCObject*);
    void onFire(CCObject*);
    void onSubmit(CCObject*);
    void onCancel(CCObject*);
public:
    static FeaturedOrderPopup* create(int level);
};
