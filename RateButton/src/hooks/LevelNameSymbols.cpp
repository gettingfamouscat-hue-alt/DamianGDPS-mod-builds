#include <Geode/Geode.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/GJGameLevel.hpp>
#include "../utils/Utils.hpp"

using namespace geode::prelude;

namespace {
    void decodeLevelName(GJGameLevel* level) {
        if (!level) return;
        std::string name = level->m_levelName;
        constexpr char const* markers[] = {"\xEE\x80\x80", "\xEE\x80\x81", "\xEE\x80\x82", "\xEE\x80\x83"};
        constexpr char replacements[] = {':', '|', '#', '~'};
        for (int i = 0; i < 4; ++i) {
            size_t pos = 0;
            while ((pos = name.find(markers[i], pos)) != std::string::npos) {
                name.replace(pos, 3, 1, replacements[i]);
                ++pos;
            }
        }
        level->m_levelName = name;
    }

    gd::string allowedLevelNameChars() {
        std::string chars;
        for (char c = 32; c <= 126; ++c) {
            chars.push_back(c);
        }
        return gd::string(chars.c_str());
    }
}

class $modify(GdpsSymbolLevel, GJGameLevel) {
    static GJGameLevel* create(cocos2d::CCDictionary* dict, bool download) {
        auto* level = GJGameLevel::create(dict, download);
        if (Utils::isOnGdps()) decodeLevelName(level);
        return level;
    }
};

class $modify(GdpsSymbolEditLevel, EditLevelLayer) {
    bool init(GJGameLevel* level) {
        if (!EditLevelLayer::init(level)) return false;
        if (!Utils::isOnGdps() || !m_textInputs) return true;
        for (auto* object : CCArrayExt<CCObject*>(m_textInputs)) {
            auto* input = typeinfo_cast<CCTextInputNode*>(object);
            if (input && level && input->getString() == level->m_levelName) {
                input->setAllowedChars(allowedLevelNameChars());
                break;
            }
        }
        return true;
    }
};
