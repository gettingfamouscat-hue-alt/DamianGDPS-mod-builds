#pragma once
#include <functional>
#include <string>

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
    static void refresh(bool force = false);
    static bool canManageRoles();
    static bool canSetDemon();
    static int accountID();
    static void assignRole(int target, std::string role, std::function<void(bool, std::string)> done);

private:
    static bool s_loaded;
};
