#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <alphalaneous.editortab_api/API.hpp>

using namespace geode::prelude;

class $modify(ClassicHalfBlockEditor, EditorUI) {
    static void onModify(auto& self) {
        (void)self.setHookPriorityPost("EditorUI::init", Priority::Late);
    }
    bool init(LevelEditorLayer* layer) {
        if (!EditorUI::init(layer)) return false;
        if (auto* mode = alpha::editor_tabs::getMode(alpha::editor_tabs::Build)) {
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
                auto* label = CCLabelBMFont::create("1/2", "bigFont.fnt");
                label->setScale(.5f);
                mode->createTab("classic-half-blocks"_spr,
                    alpha::editor_tabs::EditorTab::create(items, 0, true), label);
        }
        return true;
    }
};
