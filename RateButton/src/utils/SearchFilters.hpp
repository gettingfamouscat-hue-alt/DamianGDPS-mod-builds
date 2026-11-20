#pragma once

#include <Geode/Geode.hpp>

#include "Utils.hpp"

using namespace geode::prelude;

// Level-search difficulty subtype state (GDPS-only extras).
namespace SearchFilters {
    // Custom DemonFilterSelectLayer button tag (stock uses 0,6,7,8,9,10).
    constexpr int IMPOSSIBLE_FILTER_TAG = 11;

    // easyFilter POST values for getGJLevels.php
    constexpr int EASY_FILTER_ALL = 0;        // no param / vanilla Easy bucket
    constexpr int EASY_FILTER_VERY_EASY = 1;  // starDemonDiff = 1
    constexpr int EASY_FILTER_REGULAR = 2;    // Easy bucket excluding Very Easy

    inline constexpr char const *KEY_IMPOSSIBLE = "search-filter-impossible";
    inline constexpr char const *KEY_EASY = "search-filter-easy";

    inline bool impossibleActive() {
        return false;
    }

    inline void setImpossible(bool on) {
        Mod::get()->setSavedValue(KEY_IMPOSSIBLE, on);
    }

    inline int easyFilter() {
        return Mod::get()->getSavedValue<int>(KEY_EASY, EASY_FILTER_ALL);
    }

    inline void setEasyFilter(int value) {
        Mod::get()->setSavedValue(KEY_EASY, value);
    }

    inline void clearAll() {
        setImpossible(false);
        setEasyFilter(EASY_FILTER_ALL);
    }

    // Rewrite getGJLevels POST body for GDPS custom subtypes.
    inline gd::string patchLevelSearchParams(gd::string params) {
        if (!Utils::isOnGdps())
            return params;

        std::string p(params);

        auto setOrReplace = [&](char const *key, int value) {
            auto const needle = fmt::format("{}=", key);
            auto const piece = fmt::format("{}={}", key, value);
            auto const pos = p.find(needle);
            if (pos == std::string::npos) {
                if (!p.empty() && p.back() != '&')
                    p.push_back('&');
                p += piece;
                return;
            }
            auto end = p.find('&', pos);
            if (end == std::string::npos)
                end = p.size();
            p.replace(pos, end - pos, piece);
        };

        if (impossibleActive()) {
            // Ensure demon search + Impossible subtype (server case 6).
            setOrReplace("diff", -2);
            setOrReplace("demonFilter", 6);
        }

        auto const easy = easyFilter();
        if (easy == EASY_FILTER_VERY_EASY || easy == EASY_FILTER_REGULAR) {
            setOrReplace("easyFilter", easy);
        }

        return gd::string(p);
    }
}
