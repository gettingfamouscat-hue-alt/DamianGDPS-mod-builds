#pragma once

#include <Geode/Geode.hpp>

#include <algorithm>
#include <functional>
#include <string>
#include <utility>
#include <vector>

using namespace geode::prelude;

struct DiffPickOption {
    std::string label;
    int value = 0;
    std::string frame;     // stock sprite-frame name (empty if unused)
    std::string customSpr; // mod resource path from `_spr` (empty if unused)
};

// Shared picker used by Mod: Rate Level and Share Level Switch buttons.
class DiffPickPopup : public Popup {
protected:
    std::function<void(int)> m_onPick;

    static CCNode *makeOptionSprite(DiffPickOption const &opt) {
        constexpr float kBtnW = 210.f;
        constexpr float kBtnH = 42.f;
        constexpr float kFaceTarget = 32.f;

        auto *bg = CCScale9Sprite::create("GJ_button_01.png");
        bg->setContentSize({ kBtnW, kBtnH });

        CCSprite *face = nullptr;
        if (!opt.customSpr.empty())
            face = CCSprite::create(opt.customSpr.c_str());
        else if (!opt.frame.empty())
            face = CCSprite::createWithSpriteFrameName(opt.frame.c_str());

        if (face && face->getTexture()) {
            auto const sz = face->getContentSize();
            float const longest = std::max(sz.width, sz.height);
            if (longest > 0.f)
                face->setScale(kFaceTarget / longest);
            face->setPosition({ 26.f, kBtnH * 0.5f });
            bg->addChild(face);
        }

        auto *text = CCLabelBMFont::create(opt.label.c_str(), "goldFont.fnt");
        float const textMaxW = face ? 150.f : 190.f;
        text->limitLabelWidth(textMaxW, 0.55f, 0.32f);
        if (face) {
            // Sit to the right of the face, centered in the remaining strip.
            text->setPosition({ (kBtnW + 40.f) * 0.5f, kBtnH * 0.5f });
        } else {
            text->setPosition({ kBtnW * 0.5f, kBtnH * 0.5f });
        }
        bg->addChild(text);

        return bg;
    }

    bool init(
        char const *title,
        std::vector<DiffPickOption> const &options,
        std::function<void(int)> onPick
    ) {
        auto const win = CCDirector::sharedDirector()->getWinSize();
        int const columns = options.size() > 4 ? 2 : 1;
        int const rows = (static_cast<int>(options.size()) + columns - 1) / columns;
        float const width = std::min(columns == 2 ? 460.f : 250.f, win.width - 24.f);
        float const rowHeight = std::min(46.f, (win.height - 94.f) / std::max(1, rows));
        float const height = 70.f + rows * rowHeight;
        if (!Popup::init(width, height))
            return false;

        m_onPick = std::move(onPick);
        this->setTitle(title, "bigFont.fnt", 0.7f);
        this->setID("diff-pick-popup"_spr);

        int index = 0;
        for (auto const &opt : options) {
            auto *btn = CCMenuItemSpriteExtra::create(
                makeOptionSprite(opt),
                this,
                menu_selector(DiffPickPopup::onOption)
            );
            btn->setTag(opt.value);
            btn->setID(fmt::format("pick-{}"_spr, opt.value));
            float const columnWidth = (width - 40.f - (columns - 1) * 10.f) / columns;
            btn->setScale(std::min(columnWidth / 210.f, (rowHeight - 4.f) / 42.f));
            int const column = index % columns;
            int const row = index / columns;
            float const x = (column - (columns - 1) * .5f) * (columnWidth + 10.f);
            m_buttonMenu->addChildAtPosition(
                btn, Anchor::Top, { x, -52.f - row * rowHeight }
            );
            ++index;
        }

        return true;
    }

    void onOption(CCObject *sender) {
        auto const value = static_cast<CCNode *>(sender)->getTag();
        if (m_onPick)
            m_onPick(value);
        this->onClose(nullptr);
    }

public:
    static DiffPickPopup *create(
        char const *title,
        std::vector<DiffPickOption> const &options,
        std::function<void(int)> onPick
    ) {
        auto *ret = new DiffPickPopup();
        if (ret->init(title, options, std::move(onPick))) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};
