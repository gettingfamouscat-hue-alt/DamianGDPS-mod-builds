#include <Geode/Geode.hpp>
#include <Geode/modify/ProfilePage.hpp>
#include "../DiffPickPopup.hpp"
#include "../managers/SessionManager.hpp"
#include "../utils/Utils.hpp"

using namespace geode::prelude;

class $modify(StaffProfilePage, ProfilePage) {
    void updateRoleButton(float) {
        if (!m_mainLayer) return;
        auto* menu = typeinfo_cast<CCMenu*>(m_mainLayer->getChildByID("left-menu"));
        if (!menu) return;
        auto* existing = menu->getChildByID("manage-player-role"_spr);
        SessionManager::refresh();
        bool const show = Utils::isOnGdps() && m_score && m_accountID > 0
            && m_accountID != SessionManager::accountID() && SessionManager::canManageRoles();
        if (!show) {
            if (existing) { existing->removeFromParent(); menu->updateLayout(); }
            return;
        }
        if (existing) return;
        auto* sprite = ButtonSprite::create("Give Role", 58, true, "goldFont.fnt", "GJ_button_01.png", 22.f, .45f);
        auto* button = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(StaffProfilePage::onGiveRole));
        button->setID("manage-player-role"_spr);
        menu->addChild(button);
        menu->updateLayout();
    }

    void onGiveRole(CCObject*) {
        if (!SessionManager::canManageRoles() || !m_score || m_accountID <= 0) return;
        int const target = m_accountID;
        std::string const name = m_usernameLabel ? m_usernameLabel->getString() : std::to_string(target);
        std::vector<DiffPickOption> options{
            {"Give Admin", 0, {}, {}}, {"Give Elder Mod", 1, {}, {}},
            {"Give Mod", 2, {}, {}}, {"Remove Staff Role", 3, {}, {}},
        };
        if (auto* popup = DiffPickPopup::create("Player Role", options, [target, name](int choice) {
            static constexpr char const* roles[] = {"admin", "elder", "mod", "player"};
            static constexpr char const* labels[] = {"Admin", "Elder Mod", "Mod (send only)", "Player (no staff access)"};
            if (choice < 0 || choice > 3) return;
            std::string const role = roles[choice];
            createQuickPopup("Change Player Role",
                fmt::format("Set <cy>{}</c> to <cg>{}</c>?\nAccount ID: {}", name, labels[choice], target),
                "Cancel", "Confirm", [target, role](FLAlertLayer*, bool confirm) {
                    if (!confirm) return;
                    SessionManager::assignRole(target, role, [](bool ok, std::string message) {
                        FLAlertLayer::create(ok ? "Role Updated" : "Role Not Changed", message.c_str(), "OK")->show();
                    });
                });
        })) popup->show();
    }

    bool init(int accountID, bool ownProfile) {
        if (!ProfilePage::init(accountID, ownProfile)) return false;
        if (Utils::isOnGdps()) {
            SessionManager::refresh(true);
            schedule(schedule_selector(StaffProfilePage::updateRoleButton), .5f);
            updateRoleButton(0.f);
        }
        return true;
    }

    void loadPageFromUserInfo(GJUserScore* score) {
        ProfilePage::loadPageFromUserInfo(score);
        if (Utils::isOnGdps()) updateRoleButton(0.f);
    }

    void onExit() {
        unschedule(schedule_selector(StaffProfilePage::updateRoleButton));
        ProfilePage::onExit();
    }
};
