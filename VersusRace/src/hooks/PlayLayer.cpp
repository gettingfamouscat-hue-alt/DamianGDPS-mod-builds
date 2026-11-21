#include <Geode/Geode.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/binding/SimplePlayer.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "../managers/VersusManager.hpp"
#include "../managers/CompletionFlow.hpp"
#include <algorithm>

using namespace geode::prelude;

class $modify(VersusPlayLayer, PlayLayer) {
    struct Fields {
        CCLabelBMFont* overlay = nullptr;
        SimplePlayer* icons[2]{};
        CCLabelBMFont* names[2]{};
        CCLabelBMFont* percents[2]{};
        CCLayerColor* bars[2]{};
        CCSprite* hearts[2]{};
        CCLabelBMFont* lifeCounts[2]{};
        float pollAcc = 0.f;
        float percentAcc = 0.f;
        float deathCooldown = 0.f;
        bool endShown = false;
        float endExitDelay = -1.f;
        bool completed = false;
        bool endScreenShown = false;
        bool resultPending = false;
        bool exited = false;
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

        auto win = CCDirector::sharedDirector()->getWinSize();
        auto* hud=CCNode::create();hud->setID("versus-hud"_spr);
        hud->setPosition({win.width/2.f,win.height-28.f});addChild(hud,1000);
        for(int side=0;side<2;++side) {
            float edge=side==0?-win.width/2.f:win.width/2.f;
            float direction=side==0?1.f:-1.f;
            auto* icon=SimplePlayer::create(1);icon->setScale(.52f);
            icon->setPosition({edge+direction*27.f,-10.f});hud->addChild(icon);m_fields->icons[side]=icon;
            auto* title=CCLabelBMFont::create(side==0?"YOU":"OPPONENT","bigFont.fnt");
            title->setScale(.34f);title->setAnchorPoint({side==0?0.f:1.f,.5f});
            title->setPosition({edge+direction*55.f,6.f});hud->addChild(title);m_fields->names[side]=title;
            auto* pct=CCLabelBMFont::create("0%","bigFont.fnt");pct->setScale(.44f);
            pct->setAnchorPoint({side==0?0.f:1.f,.5f});
            pct->setPosition({edge+direction*55.f,-17.f});hud->addChild(pct);m_fields->percents[side]=pct;
            auto* heart=CCSprite::create("versus-heart.png"_spr);
            if(heart){heart->setScale(.31f);heart->setPosition({edge+direction*110.f,-17.f});hud->addChild(heart);}
            m_fields->hearts[side]=heart;
            auto* lifeCount=CCLabelBMFont::create("3","bigFont.fnt");lifeCount->setScale(.4f);
            lifeCount->setPosition({edge+direction*138.f,-17.f});hud->addChild(lifeCount);m_fields->lifeCounts[side]=lifeCount;
            float trackWidth=std::max(70.f,win.width/2.f-115.f);
            float trackX=side==0?-win.width/2.f+110.f:5.f;
            auto* track=CCLayerColor::create({20,20,25,255},trackWidth,5.f);
            track->setPosition({trackX,23.f});hud->addChild(track);
            auto* bar=CCLayerColor::create(side==0?ccc4(65,230,245,255):ccc4(255,105,70,255),1.f,5.f);
            bar->setPosition({trackX,23.f});hud->addChild(bar);m_fields->bars[side]=bar;
        }
        m_fields->overlay=m_fields->names[0];
        this->refreshOverlay();

        VersusManager::get().setListener([this](VersusSnapshot const& snap) {
            this->refreshOverlay();
            this->maybeShowEnd(snap);
        });

        return true;
    }

    void refreshOverlay() {
        if(!m_fields->overlay)return;
        auto snap=VersusManager::get().last();
        snap.myPercent=this->getCurrentPercent();
        auto* gm=GameManager::sharedState();
        for(int side=0;side<2;++side) {
            int icon=side==0?snap.myIcon:snap.oppIcon;
            int c1=side==0?snap.myColor1:snap.oppColor1;
            int c2=side==0?snap.myColor2:snap.oppColor2;
            auto* player=m_fields->icons[side];
            if(player) {
                player->updatePlayerFrame(std::max(1,icon),IconType::Cube);
                if(gm)player->setColors(gm->colorForIdx(c1),gm->colorForIdx(c2));
            }
            auto* title=m_fields->names[side];
            if(title) {
                title->setString(side==0?"YOU":(snap.oppName.empty()?"OPPONENT":snap.oppName.c_str()));
                title->limitLabelWidth(94.f,.32f,.18f);
            }
            float percent=std::clamp(side==0?snap.myPercent:snap.oppPercent,0.f,100.f);
            auto* percentLabel=m_fields->percents[side];
            if(percentLabel)percentLabel->setString(fmt::format("{:.0f}%",percent).c_str());
            if(auto* bar=m_fields->bars[side])bar->setContentSize({std::max(1.f,(CCDirector::sharedDirector()->getWinSize().width/2.f-115.f)*percent/100.f),5.f});
            int remaining=std::clamp(side==0?snap.myLives:snap.oppLives,0,3);
            auto* heart=m_fields->hearts[side];
            auto* count=m_fields->lifeCounts[side];
            if(heart)heart->setVisible(remaining>0);
            if(count)count->setString(std::to_string(remaining).c_str());
            // Keep the heart and life count next to the actual percent text on either side.
            float edge=side==0?-CCDirector::sharedDirector()->getWinSize().width/2.f
                              :CCDirector::sharedDirector()->getWinSize().width/2.f;
            float direction=side==0?1.f:-1.f;
            float offset=55.f;
            if(percentLabel)offset+=percentLabel->getContentSize().width*percentLabel->getScaleX();
            if(heart) {
                float heartWidth=heart->getContentSize().width*heart->getScaleX();
                heart->setPosition({edge+direction*(offset+4.f+heartWidth/2.f),-17.f});
                offset+=4.f+heartWidth;
            }
            if(count) {
                float countWidth=count->getContentSize().width*count->getScaleX();
                count->setPosition({edge+direction*(offset+3.f+countWidth/2.f),-17.f});
            }
        }
    }

    void maybeShowEnd(VersusSnapshot const& snap) {
        if (m_fields->endShown || !m_fields->overlay || snap.matchID != m_fields->matchID)
            return;
        if (!(snap.youWin || snap.youLose || snap.phase == "finished"))
            return;
        m_fields->pendingResult = snap;
        m_fields->resultPending = true;
        if (m_fields->completed && !m_fields->endScreenShown)
            return;
        m_fields->endShown = true;
        m_fields->resultPending = false;
        m_fields->endExitDelay = 3.f;

        char const* title = snap.youWin ? "You Win!" : "You Lose!";
        char const* body = snap.youWin
            ? "Nice race — you won the versus match."
            : "Out of lives (or opponent finished first).";

        // Keep the native completion animation intact. Once the result is
        // acknowledged, use the game's normal exit path to return to the menu.
        geode::Ref<VersusPlayLayer> self = this;
        createQuickPopup(title, body, "OK", nullptr, [self](FLAlertLayer*, bool) {
            if (!self->m_fields->exited) self->onQuit();
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
        if (m_fields->endShown && m_fields->endExitDelay >= 0.f && !m_fields->exited) {
            m_fields->endExitDelay -= dt;
            if (m_fields->endExitDelay < 0.f) { this->onQuit(); return; }
        }
        if (m_fields->overlay && !m_fields->endShown && VersusManager::get().last().phase == "finished")
            this->maybeShowEnd(VersusManager::get().last());

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
        m_fields->exited = true;
        // Scene changes can bypass onQuit. Never keep a listener that owns a
        // raw pointer to a PlayLayer after the layer leaves the scene.
        if (m_fields->overlay && VersusManager::get().matchID() == m_fields->matchID)
            VersusManager::get().clearListener();
        PlayLayer::onExit();
    }
};
