#pragma once
#include <Geode/Geode.hpp>
#include <vector>
using namespace geode::prelude;
class AdminDemonPopup : public Popup {
    int m_level = 0, m_selected = 2;
    bool m_ready = false, m_chosen = false, m_pending = false;
    CCMenuItemSpriteExtra* m_submit = nullptr;
    std::vector<CCMenuItemSpriteExtra*> m_faces;
    bool init(int level);
    void update();
    void onPick(CCObject*);
    void onSubmit(CCObject*);
    void onCancel(CCObject*);
public:
    static AdminDemonPopup* create(int level);
};
