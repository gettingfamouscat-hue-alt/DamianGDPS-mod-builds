#pragma once

class SessionManager {
public:
    static bool ShownGDPSWarning;
    static bool ShownRateButtonWarning;

    // From requestUserAccess: 0 = none, 1 = mod (send), 2 = elder (rate)
    static int ModAccessLevel;

    static void ensureLoaded();
    static bool canUseRateButton();
    static bool isElderMod();
    static void setModAccessLevel(int level);

private:
    static bool s_loaded;
};
