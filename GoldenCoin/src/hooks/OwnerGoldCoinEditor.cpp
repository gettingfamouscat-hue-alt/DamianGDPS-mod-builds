#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include "../utils/CoinContext.hpp"

using namespace geode::prelude;

class OwnerGoldCoinPopup : public Popup {
    EffectGameObject* m_coin = nullptr;
    LevelEditorLayer* m_editor = nullptr;
    CCMenuItemToggler* m_reward = nullptr;

    bool init(EffectGameObject* coin, LevelEditorLayer* editor) {
        if (!Popup::init(320.f, 170.f)) return false;
        m_coin = coin;
        m_editor = editor;
        setTitle("Gold Coin");
        m_reward = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(OwnerGoldCoinPopup::onToggle), .7f);
        m_reward->toggle(coin->m_secretCoinID >= 1 && coin->m_secretCoinID <= 3);
        m_reward->setPosition({44.f, 84.f});
        m_buttonMenu->addChild(m_reward);
        auto* label = CCLabelBMFont::create("Give 1 secret coin", "bigFont.fnt");
        label->setScale(.42f);
        label->setAnchorPoint({0.f, .5f});
        label->setPosition({72.f, 84.f});
        m_mainLayer->addChild(label);
        auto* note = CCLabelBMFont::create("Awarded once after the level is beaten", "chatFont.fnt");
        note->setScale(.52f);
        note->setPosition({160.f, 54.f});
        m_mainLayer->addChild(note);
        auto* save = CCMenuItemSpriteExtra::create(ButtonSprite::create("Save"),
            this, menu_selector(OwnerGoldCoinPopup::onSave));
        save->setScale(.7f);
        save->setPosition({160.f, 25.f});
        m_buttonMenu->addChild(save);
        return true;
    }

    void onToggle(CCObject*) {}
    void onSave(CCObject*) {
        if (!m_coin || !m_editor) return;
        if (!m_reward->isToggled()) {
            m_coin->m_secretCoinID = 0;
        } else if (m_coin->m_secretCoinID < 1 || m_coin->m_secretCoinID > 3) {
            bool used[4] = {};
            if (auto* objects = m_editor->getAllObjects()) {
                for (auto* object : CCArrayExt<GameObject*>(objects)) {
                    if (object == m_coin || !object || object->m_objectID != 142) continue;
                    if (auto* other = typeinfo_cast<EffectGameObject*>(object)) {
                        if (other->m_secretCoinID >= 1 && other->m_secretCoinID <= 3)
                            used[other->m_secretCoinID] = true;
                    }
                }
            }
            int slot = 1;
            while (slot <= 3 && used[slot]) ++slot;
            if (slot > 3) {
                FLAlertLayer::create("Gold Coin", "A level can award up to 3 secret coins.", "OK")->show();
                return;
            }
            m_coin->m_secretCoinID = slot;
        }
        onClose(nullptr);
    }
public:
    static OwnerGoldCoinPopup* create(EffectGameObject* coin, LevelEditorLayer* editor) {
        auto* popup = new OwnerGoldCoinPopup();
        if (popup->init(coin, editor)) { popup->autorelease(); return popup; }
        delete popup;
        return nullptr;
    }
};

class $modify(OwnerGoldCoinEditor, EditorUI) {
    struct Fields {
        CCObject* originalSpecialTarget = nullptr;
        SEL_MenuHandler originalSpecialSelector = nullptr;
        bool specialCaptured = false;
    };

    bool selectedOwnerCoin() const {
        return GoldenCoin::isOnGdps() && GoldenCoin::accountID() == 1
            && m_selectedObject && m_selectedObject->m_objectID == 142;
    }

    void onOwnerCoinSpecial(CCObject*) {
        if (!selectedOwnerCoin()) return;
        if (auto* coin = typeinfo_cast<EffectGameObject*>(m_selectedObject))
            if (auto* popup = OwnerGoldCoinPopup::create(coin, m_editorLayer)) popup->show();
    }

    void editObject(CCObject* sender) {
        if (!selectedOwnerCoin()) {
            EditorUI::editObject(sender);
            return;
        }
        if (auto* popup = CustomizeObjectLayer::create(m_selectedObject, nullptr)) popup->show();
    }

    void updateButtons() {
        EditorUI::updateButtons();
        if (!m_editSpecialBtn) return;
        if (!m_fields->specialCaptured) {
            m_fields->originalSpecialTarget = m_editSpecialBtn->m_pListener;
            m_fields->originalSpecialSelector = m_editSpecialBtn->m_pfnSelector;
            m_fields->specialCaptured = true;
        }
        if (selectedOwnerCoin()) {
            m_editSpecialBtn->setTarget(this, menu_selector(OwnerGoldCoinEditor::onOwnerCoinSpecial));
            m_editSpecialBtn->setVisible(true);
            m_editSpecialBtn->setEnabled(true);
            if (m_editObjectBtn) {
                m_editObjectBtn->setVisible(true);
                m_editObjectBtn->setEnabled(true);
            }
        } else if (m_fields->originalSpecialTarget && m_fields->originalSpecialSelector) {
            m_editSpecialBtn->setTarget(m_fields->originalSpecialTarget,
                m_fields->originalSpecialSelector);
        }
    }

    bool init(LevelEditorLayer* layer) {
        if (!EditorUI::init(layer)) return false;
        // Hide the palette from every other account. The upload endpoint also
        // checks the authenticated account, so a modified client cannot add it.
        if (!GoldenCoin::isOnGdps() || GoldenCoin::accountID() != 1) return true;

        alpha::editor_tabs::addTab("owner-gold-coin"_spr, alpha::editor_tabs::BUILD,
            [this] {
                auto* items = CCArray::create();
                if (GoldenCoin::accountID() == 1 && GoldenCoin::isOnGdps()) {
                    if (auto* button = this->getCreateBtn(142, 4)) items->addObject(button);
                }
                return EditButtonBar::create(items, {}, -1, false,
                    GameManager::get()->getIntGameVariable("0049"),
                    GameManager::get()->getIntGameVariable("0050"));
            },
            [] {
                auto* coin = CCSprite::createWithSpriteFrameName("secretCoin_01_001.png");
                if (coin) coin->setScale(.75f);
                return coin;
            });
        return true;
    }
};
