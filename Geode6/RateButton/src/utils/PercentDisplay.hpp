#pragma once

#include <Geode/Geode.hpp>
#include <string>

namespace PercentDisplay {
    inline bool frozen(float percent) {
        return percent >= 64.f && percent < 68.f;
    }

    inline void capLabels(cocos2d::CCNode* node) {
        if (!node) return;
        if (auto* label = geode::prelude::typeinfo_cast<cocos2d::CCLabelBMFont*>(node)) {
            std::string value = label->getString();
            for (int percent = 65; percent <= 67; ++percent) {
                auto const from = std::to_string(percent) + "%";
                auto const pos = value.find(from);
                if (pos != std::string::npos && (pos == 0 || value[pos - 1] < '0' || value[pos - 1] > '9')) {
                    value.replace(pos, from.size(), "64%");
                    label->setString(value.c_str());
                    break;
                }
            }
        }
        if (node->getChildren()) {
            for (auto* child : node->getChildrenExt()) capLabels(child);
        }
    }

    inline void capExactLabels(cocos2d::CCNode* node, cocos2d::CCNode* exclude = nullptr) {
        if (!node || node == exclude) return;
        if (auto* label = geode::prelude::typeinfo_cast<cocos2d::CCLabelBMFont*>(node)) {
            std::string value = label->getString();
            if (value == "65%" || value == "66%" || value == "67%") label->setString("64%");
        }
        if (node->getChildren()) {
            for (auto* child : node->getChildrenExt()) capExactLabels(child, exclude);
        }
    }
}
