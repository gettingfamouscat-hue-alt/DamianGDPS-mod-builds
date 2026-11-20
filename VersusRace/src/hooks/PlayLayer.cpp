#include <Geode/Geode.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "../managers/VersusManager.hpp"
#include "../managers/CompletionFlow.hpp"

using namespace geode::prelude;

class $modify(VersusPlayLayer, PlayLayer) {
    struct Fields {
        CCLabelBMFont* overlay = nullptr;
        float pollAcc = 0.f;
        float percentAcc = 0.f;
        float deathCooldown = 0.f;
        bool endShown = false;
        bool completed = false;
        bool endScreenShown = false;
        bool resultPending = false;
        int matchID = 0;
        VersusSnapshot pendingResult;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;

        if (!VersusManager::get().inMatch()
            || VersusManager::get().levelID() != static_cast<int>(level->m_levelID))
            return true;
        m_fields->matchID = VersusManager::get().matchID();

        VersusManager::get().markInGame();

        auto* label = CCLabelBMFont::create("VS loading…", "bigFont.fnt");
        label->setScale(0.32f);
        label->setAnchorPoint({ 0.5f, 1.f });
        label->setID("versus-overlay"_spr);
        this->addChild(label, 1000);
        auto win = CCDirector::sharedDirector()->getWinSize();
        label->setPosition({ win.width / 2.f, win.height - 6.f });
        m_fields->overlay = label;
        this->refreshOverlay();

        VersusManager::get().setListener([this](VersusSnapshot const& snap) {
            this->refreshOverlay();
            this->maybeShowEnd(snap);
        });

        return true;
    }

    void refreshOverlay() {
        auto* label = m_fields->overlay;
        if (!label)
            return;
        auto s = VersusManager::get().last();
        // Prefer live local percent for "you"
        s.myPercent = this->getCurrentPercent();
        label->setString(fmt::format(
            "YOU {:.0f}% L{}   |   {} {:.0f}% L{}",
            s.myPercent,
            s.myLives,
            s.oppName.empty() ? "OPP" : s.oppName.c_str(),
            s.oppPercent,
            s.oppLives
        ).c_str());
    }

    void maybeShowEnd(VersusSnapshot const& snap) {
        if (m_fields->endShown || !m_fields->overlay || snap.matchID != m_fields->matchID)
            return;
        if (!(snap.youWin || snap.youLose || snap.phase == "finished"))
            return;
        m_fields->pendingResult = snap;
        m_fields->resultPending = true;
        if (CompletionFlow::deferResult(m_levelEndAnimationStarted, m_fields->completed, m_fields->endScreenShown))
            return;
        m_fields->endShown = true;
        m_fields->resultPending = false;

        char const* title = snap.youWin ? "You Win!" : "You Lose!";
        char const* body = snap.youWin
            ? "Nice race — you won the versus match."
            : "Out of lives (or opponent finished first).";

        createQuickPopup(title, body, "OK", nullptr, [](FLAlertLayer*, bool) {
            VersusManager::get().clearListener();
            VersusManager::get().leaveMatch();
        });
    }

    void levelComplete() {
        bool const race = m_fields->overlay && !m_isPracticeMode;
        if (race) m_fields->completed = true;
        PlayLayer::levelComplete();
        if (race) VersusManager::get().reportPercent(100.f);
    }

    void showEndLayer() {
        PlayLayer::showEndLayer();
        m_fields->endScreenShown = true;
        if (m_fields->resultPending) maybeShowEnd(m_fields->pendingResult);
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        if (!m_fields->overlay || !VersusManager::get().inMatch() || m_fields->endShown)
            return;

        if (m_fields->deathCooldown > 0.f)
            m_fields->deathCooldown -= dt;

        m_fields->percentAcc += dt;
        m_fields->pollAcc += dt;

        if (m_fields->percentAcc >= 0.75f) {
            m_fields->percentAcc = 0.f;
            VersusManager::get().reportPercent(CompletionFlow::reportablePercent(
                this->getCurrentPercent(), m_fields->completed));
            this->refreshOverlay();
        }
        if (m_fields->pollAcc >= 1.0f) {
            m_fields->pollAcc = 0.f;
            VersusManager::get().pollState();
        }
    }

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        bool wasDead = m_player1 ? m_player1->m_isDead : false;

        PlayLayer::destroyPlayer(player, object);

        if (!m_fields->overlay || !VersusManager::get().inMatch() || m_fields->endShown)
            return;
        if (m_fields->deathCooldown > 0.f)
            return;

        if (player == m_player1 && m_player1 && m_player1->m_isDead && !wasDead) {
            m_fields->deathCooldown = 1.25f;
            VersusManager::get().reportDeath();
            this->refreshOverlay();
        }
    }

    void onQuit() {
        if (m_fields->overlay && VersusManager::get().matchID() == m_fields->matchID) {
            VersusManager::get().clearListener();
            if (VersusManager::get().inMatch())
                VersusManager::get().leaveMatch();
        }
        PlayLayer::onQuit();
    }

    void onExit() {
        // Scene changes can bypass onQuit. Never keep a listener that owns a
        // raw pointer to a PlayLayer after the layer leaves the scene.
        if (m_fields->overlay && VersusManager::get().matchID() == m_fields->matchID)
            VersusManager::get().clearListener();
        PlayLayer::onExit();
    }
};
