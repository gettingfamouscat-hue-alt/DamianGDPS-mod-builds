#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>

using namespace geode::prelude;

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
