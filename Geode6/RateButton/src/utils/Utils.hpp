#pragma once

#include <string_view>

using namespace geode::prelude;

namespace Utils {
    constexpr int IMPOSSIBLE_DEMON_DIFF = 7; // server starDemonDiff
    constexpr int DEMON_SUBTYPE_COUNT = 5;   // Easy…Extreme + Impossible (picker index)
    constexpr int IMPOSSIBLE_DEMON_INDEX = 5;

    /*
        Very Easy storage (GDPS custom, non-colliding):
          starDifficulty = 10  (Easy — vanilla face / Easy search bucket)
          starAuto       = 0
          starDemon      = 0
          starDemonDiff  = 1   (VERY_EASY_DIFF_FLAG)

        Vanilla ignores starDemonDiff when starDemon=0.
        Demon subtypes use 0/3/4/5/6/7 — value 1 is unused there.
        Rate Button overlays very_easy.png when !demon && demonDiff==1.
    */
    constexpr int VERY_EASY_DIFF_FLAG = 1;
    constexpr int EASY_STARS = 2; // Easy family in the star picker
    constexpr int GRANDPA_FIRST_STARS = 11;
    constexpr int GRANDPA_LAST_STARS = 16;
    constexpr int GRANDPA_FIRST_DIFF = 8; // GDPS starDemonDiff, after legacy 7

    inline constexpr char const* grandpaLabels[] = {
        "Supreme Demon", "Ultimate Demon", "Legendary Demon",
        "Mythical Demon", "Silent Demon", "Grandpa Demon"
    };

    inline constexpr char const* grandpaCommands[] = {
        "supreme-demon", "ultimate-demon", "legendary-demon",
        "mythical-demon", "silent-demon", "grandpa-demon"
    };

    inline constexpr char const* grandpaFrames[] = {
        "ultrasoda.grandpa_demon_revived/GrD_demon0.png",
        "ultrasoda.grandpa_demon_revived/GrD_demon1.png",
        "ultrasoda.grandpa_demon_revived/GrD_demon2.png",
        "ultrasoda.grandpa_demon_revived/GrD_demon3.png",
        "ultrasoda.grandpa_demon_revived/GrD_demon4.png",
        "ultrasoda.grandpa_demon_revived/GrD_demon5.png"
    };

    inline int grandpaIndex(GJGameLevel* level) {
        if (!level || !level->m_demon) return -1;
        int const index = static_cast<int>(level->m_demonDifficulty) - GRANDPA_FIRST_DIFF;
        return index >= 0 && index < 6 ? index : -1;
    }

    constexpr std::string_view getLevelDifficulty(int stars) {
        constexpr std::string_view difficulties[] = {
            "na", "auto", "easy", "normal", "hard", "hard",
            "harder", "harder", "insane", "insane", "demon",
            "supreme-demon", "ultimate-demon", "legendary-demon",
            "mythical-demon", "silent-demon", "grandpa-demon"
        };

        return (stars >= 0 && stars <= GRANDPA_LAST_STARS) ? difficulties[stars] : "na";
    }

    // 0=Easy, 1=Medium, 2=Hard, 3=Insane, 4=Extreme, 5=Impossible
    constexpr std::string_view getDemonDifficultyName(int demonDiff) {
        constexpr std::string_view names[] = {
            "easy-demon", "medium-demon", "hard-demon",
            "insane-demon", "extreme-demon"
        };
        if (demonDiff < 0 || demonDiff >= DEMON_SUBTYPE_COUNT) return "hard-demon";
        return names[demonDiff];
    }

    constexpr bool isCustomDemonFace(int demonDiff) {
        return false;
    }

    constexpr char const* getDemonFaceFrame(int demonDiff) {
        constexpr char const* frames[] = {
            "difficulty_07_btn_001.png", // Easy Demon
            "difficulty_08_btn_001.png", // Medium Demon
            "difficulty_06_btn_001.png", // Hard Demon
            "difficulty_09_btn_001.png", // Insane Demon
            "difficulty_10_btn_001.png", // Extreme Demon
            nullptr                        // Impossible — custom resource
        };
        if (demonDiff < 0 || demonDiff >= DEMON_SUBTYPE_COUNT) return frames[2];
        return frames[demonDiff];
    }

    constexpr char const* getDemonDisplayLabel(int demonDiff) {
        constexpr char const* labels[] = {
            "Easy Demon", "Medium Demon", "Hard Demon",
            "Insane Demon", "Extreme Demon"
        };
        if (demonDiff < 0 || demonDiff >= DEMON_SUBTYPE_COUNT) return labels[2];
        return labels[demonDiff];
    }

    // Face-only picker: 0=N/A … 6=Insane … 7-12=demon subtypes (incl. Impossible)
    constexpr int FACE_DIFF_COUNT = 12;

    constexpr std::string_view getFaceOnlyDifficultyName(int index) {
        constexpr std::string_view names[] = {
            "na", "auto", "easy", "normal", "hard", "harder", "insane",
            "easy-demon", "medium-demon", "hard-demon", "insane-demon",
            "extreme-demon"
        };
        if (index < 0 || index >= FACE_DIFF_COUNT) return "na";
        return names[index];
    }

    constexpr char const* getFaceOnlyFrame(int index) {
        constexpr char const* frames[] = {
            "difficulty_00_btn_001.png",
            "difficulty_auto_btn_001.png",
            "difficulty_01_btn_001.png",
            "difficulty_02_btn_001.png",
            "difficulty_03_btn_001.png",
            "difficulty_04_btn_001.png",
            "difficulty_05_btn_001.png",
            "difficulty_07_btn_001.png",
            "difficulty_08_btn_001.png",
            "difficulty_06_btn_001.png",
            "difficulty_09_btn_001.png",
            "difficulty_10_btn_001.png",
            nullptr // Impossible — custom resource
        };
        if (index < 0 || index >= FACE_DIFF_COUNT) return frames[0];
        return frames[index];
    }

    inline bool isVeryEasyLevel(GJGameLevel *level) {
        return level
            && !level->m_demon
            && static_cast<int>(level->m_demonDifficulty) == VERY_EASY_DIFF_FLAG;
    }

    inline bool isImpossibleDemonLevel(GJGameLevel *level) {
return false;
    }

    // Readable name for a rated level (cells / info face caption).
    inline char const *getRatedDifficultyLabel(GJGameLevel *level) {
        if (!level)
            return "N/A";
        if (isVeryEasyLevel(level))
            return "Very Easy";
        if (level->m_autoLevel)
            return "Auto";
        if (level->m_demon) {
            if (int const index = grandpaIndex(level); index >= 0)
                return grandpaLabels[index];
            switch (static_cast<int>(level->m_demonDifficulty)) {
                case 3: return "Easy Demon";
                case 4: return "Medium Demon";
                case 0: return "Hard Demon";
                case 5: return "Insane Demon";
                case 6: return "Extreme Demon";
                case 7: return "Extreme Demon";
                default: return "Demon";
            }
        }
        switch (static_cast<int>(level->m_stars)) {
            case 1: return "Auto";
            case 2: return "Easy";
            case 3: return "Normal";
            case 4:
            case 5: return "Hard";
            case 6:
            case 7: return "Harder";
            case 8:
            case 9: return "Insane";
            case 10: return "Demon";
            default: return "N/A";
        }
    }

    // Creator request (upload ShareLevelLayer stars only — no subtype on server).
    // Optional picker overrides for local share UI / saved draft.
    inline char const *getRequestedDifficultyLabel(
        int stars,
        int easyVariant = 0,
        int demonPicker = 4
    ) {
        if (stars <= 0)
            return "N/A";
        if (stars == EASY_STARS && easyVariant == 1)
            return "Very Easy";
        if (stars == 10)
            return getDemonDisplayLabel(demonPicker);
        switch (stars) {
            case 1: return "Auto";
            case 2: return "Easy";
            case 3: return "Normal";
            case 4:
            case 5: return "Hard";
            case 6:
            case 7: return "Harder";
            case 8:
            case 9: return "Insane";
            default: return "N/A";
        }
    }

    bool isOnGdps();
    std::string serverBaseUrl();
}
