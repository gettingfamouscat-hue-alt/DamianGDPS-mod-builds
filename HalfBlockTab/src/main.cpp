#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GameObject.hpp>
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>

using namespace geode::prelude;

class $modify(SeamlessPlankFill, GameObject) {
    static GameObject* createWithKey(int key) {
        auto* object = GameObject::createWithKey(key);
        if (!object || key != 1903 || !object->m_colorSprite) return object;

        // The stock color layer is 29 units wide inside a 30-unit outline.
        // Use a full-bleed texture so neighboring slabs have no clear strip.
        auto* fill = object->m_colorSprite;
        auto oldSize = fill->getContentSize();
        float oldScaleX = fill->getScaleX();
        float oldScaleY = fill->getScaleY();
        auto* texture = CCTextureCache::sharedTextureCache()->addImage("seamless-plank-fill.png"_spr, false);
        if (!texture) return object;
        auto textureSize = texture->getContentSize();
        if (textureSize.width <= 0.f || textureSize.height <= 0.f) return object;
        fill->setTexture(texture);
        fill->setTextureRect({0.f, 0.f, textureSize.width, textureSize.height});
        fill->setScaleX(oldScaleX * (oldSize.width * 30.f / 29.f) / textureSize.width);
        fill->setScaleY(oldScaleY * oldSize.height / textureSize.height);
        return object;
    }
};

class $modify(ClassicHalfBlockEditor, EditorUI) {
    bool init(LevelEditorLayer* layer) {
        if (!EditorUI::init(layer)) return false;

        alpha::editor_tabs::addTab("classic-half-blocks"_spr, alpha::editor_tabs::BUILD,
            [this] {
                auto* items = CCArray::create();
                // Colorable 2.1/2.2 slabs and quarter blocks, then their
                // original black versions which GD removed from the palette.
                constexpr int objectIDs[] = {
                    1903, 1904, 1905, 1910, 1911,
                    40, 369, 370, 195, 196
                };
                for (int id : objectIDs) {
                    if (auto* button = this->getCreateBtn(id, 4)) {
                        items->addObject(button);
                    }
                }
                return EditButtonBar::create(items, {}, -1, false,
                    GameManager::get()->getIntGameVariable("0049"),
                    GameManager::get()->getIntGameVariable("0050"));
            },
            [] {
                auto* label = CCLabelBMFont::create("1/2", "bigFont.fnt");
                label->setScale(.5f);
                return label;
            });
        return true;
    }
};
