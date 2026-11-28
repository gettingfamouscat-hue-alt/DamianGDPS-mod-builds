#include <Geode/modify/InfoLayer.hpp>
#include "../managers/CreatorDisplay.hpp"
#include "../utils/Utils.hpp"

using namespace geode::prelude;

class $modify(CreatorInfoLayer, InfoLayer) {
    struct Fields { CreatorDisplay::State display; };

    // Older Node IDs releases do not name this header. Find only the uploader's
    // exact header text, before any comments have been loaded.
    CCLabelBMFont* findCreatorHeader(CCNode* node) {
        if (auto* label = typeinfo_cast<CCLabelBMFont*>(node)) {
            std::string text = label->getString();
            std::string creator = m_level->m_creatorName;
            if (text == "By " + creator || text == "BY " + creator
                || text == "by " + creator) return label;
        }
        for (auto* child : CCArrayExt<CCNode*>(node->getChildren()))
            if (auto* label = findCreatorHeader(child)) return label;
        return nullptr;
    }

    void updateCreator(float = 0.f) {
        if (m_level && Utils::isOnGdps())
            CreatorDisplay::apply(this, m_level->m_levelID, m_fields->display, false);
    }

    bool init(GJGameLevel* level, GJUserScore* score, GJLevelList* list, GJEditorSoundConfig* sound) {
        if (!InfoLayer::init(level, score, list, sound)) return false;
        if (level && Utils::isOnGdps()) {
            if (!getChildByIDRecursive("creator-name")) {
                if (auto* label = findCreatorHeader(this)) {
                    auto* parent = label->getParent();
                    if (typeinfo_cast<CCMenuItem*>(parent)) parent->setID("creator-name");
                    else label->setID("creator-name");
                }
            }
            updateCreator();
        }
        return true;
    }
    void onEnter() {
        InfoLayer::onEnter();
        if (m_level && Utils::isOnGdps()) {
            updateCreator();
            schedule(schedule_selector(CreatorInfoLayer::updateCreator), .5f);
        }
    }
    void onExit() {
        unschedule(schedule_selector(CreatorInfoLayer::updateCreator));
        InfoLayer::onExit();
    }
};
