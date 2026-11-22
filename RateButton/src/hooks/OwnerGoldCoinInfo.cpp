#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/utils/web.hpp>
#include <thread>
#include <map>
#include "../utils/Utils.hpp"
using namespace geode::prelude;
namespace {
std::map<std::string, int> goldCoinCounts;
std::string requestGoldCoins(int levelID) {
    auto key = Utils::serverBaseUrl() + "/goldcoin/status.php?levelID=" + std::to_string(levelID);
    if (goldCoinCounts.contains(key)) return key;
    goldCoinCounts[key] = -1;
    std::thread([key] {
        auto request = web::WebRequest();
        request.timeout(std::chrono::seconds(8));
        auto response = request.getSync(key);
        auto parsed = response.json();
        int count = response.ok() && parsed ? parsed.unwrap()["count"].asInt().unwrapOr(0) : 0;
        Loader::get()->queueInMainThread([key, count] { goldCoinCounts[key] = count; });
    }).detach();
    return key;
}
void restorePendingCoinArt(CCNode* node) {
    if (node->getUserObject("owner-gold-pending"_spr)) {
        node->setVisible(true);
        node->setUserObject("owner-gold-pending"_spr, nullptr);
    }
    if (auto* children = node->getChildren())
        for (auto* child : CCArrayExt<CCNode*>(children)) restorePendingCoinArt(child);
}
void replaceCoinArt(CCNode* node, bool pending = false) {
    if (auto* sprite = typeinfo_cast<CCSprite*>(node)) {
        for (auto pair : {std::pair{"secretCoinUI2_001.png", "secretCoinUI_001.png"},
                std::pair{"usercoin_small01_001.png", "GJ_coinsIcon_001.png"},
                std::pair{"secretCoin_2_01_001.png", "secretCoin_01_001.png"},
                std::pair{"secretCoin_2_01_002.png", "secretCoin_01_002.png"},
                std::pair{"secretCoin_2_01_003.png", "secretCoin_01_003.png"},
                std::pair{"secretCoin_2_01_004.png", "secretCoin_01_004.png"},
                std::pair{"secretCoin_2_b_01_001.png", "secretCoin_b_01_001.png"},
                std::pair{"secretCoin_2_b_01_002.png", "secretCoin_b_01_002.png"},
                std::pair{"secretCoin_2_b_01_003.png", "secretCoin_b_01_003.png"},
                std::pair{"secretCoin_2_b_01_004.png", "secretCoin_b_01_004.png"},
                std::pair{"GJ_coinsIcon2_001.png", "GJ_coinsIcon_001.png"},
                std::pair{"GJ_coinsIcon_001.png", "GJ_coinsIcon_001.png"},
                std::pair{"GJ_coinsIcon_gray_001.png", "GJ_coinsIcon_001.png"}}) {
            auto* oldFrame = CCSpriteFrameCache::get()->spriteFrameByName(pair.first);
            auto* newFrame = CCSpriteFrameCache::get()->spriteFrameByName(pair.second);
            if (oldFrame && newFrame && sprite->getTexture() == oldFrame->getTexture()
                && sprite->getTextureRect().equals(oldFrame->getRect())) {
                if (pending) {
                    if (sprite->isVisible()) {
                        sprite->setUserObject("owner-gold-pending"_spr, CCBool::create(true));
                        sprite->setVisible(false);
                    }
                    break;
                }
                auto width = sprite->getContentSize().width * sprite->getScaleX();
                sprite->setDisplayFrame(newFrame);
                sprite->setColor(ccWHITE);
                sprite->setScale(width / sprite->getContentSize().width);
                break;
            }
        }
    }
    if (auto* children = node->getChildren())
        for (auto* child : CCArrayExt<CCNode*>(children)) replaceCoinArt(child, pending);
}
class $modify(OwnerGoldCoinEndScreen, EndLevelLayer) {
    void customSetup() {
        auto* play = PlayLayer::get();
        bool gold = Utils::isOnGdps() && play && play->m_level && play->getUserObject("owner-gold-level"_spr);
        auto originalType = gold ? play->m_level->m_levelType : static_cast<GJLevelType>(0);
        if (gold) {
            play->m_level->m_levelType = GJLevelType::Main;
            m_notLocal = false;
        }
        EndLevelLayer::customSetup();
        if (!gold) return;
        play->m_level->m_levelType = originalType;
        m_notLocal = false;
        replaceCoinArt(this);
        schedule(schedule_selector(OwnerGoldCoinEndScreen::refreshEndCoins), .05f);
    }
    void refreshEndCoins(float) { replaceCoinArt(this); }
};
}
class $modify(OwnerGoldCoinCell, LevelCell) {
    struct Fields { std::string key; bool logged = false; };
    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);
        if (!Utils::isOnGdps() || !level || static_cast<int>(level->m_levelID) <= 0) return;
        m_fields->key = requestGoldCoins(static_cast<int>(level->m_levelID));
        if (goldCoinCounts[m_fields->key] < 0) replaceCoinArt(this, true);
        else if (goldCoinCounts[m_fields->key] > 0) replaceCoinArt(this);
        schedule(schedule_selector(OwnerGoldCoinCell::refreshGoldArt), .05f);
    }
    void refreshGoldArt(float) {
        if (m_fields->key.empty()) return;
        if (!m_fields->logged && goldCoinCounts[m_fields->key] >= 0) {
            log::info("Gold coin list status: {} = {}", m_fields->key, goldCoinCounts[m_fields->key]);
            m_fields->logged = true;
        }
        if (goldCoinCounts[m_fields->key] >= 0) restorePendingCoinArt(this);
        if (goldCoinCounts[m_fields->key] > 0) replaceCoinArt(this);
    }
};
class $modify(OwnerGoldCoinInfo, LevelInfoLayer) {
    struct Fields { std::string key; bool drawn = false; };
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;
        if (!Utils::isOnGdps() || !level || static_cast<int>(level->m_levelID) <= 0) return true;
        m_fields->key = requestGoldCoins(static_cast<int>(level->m_levelID));
        schedule(schedule_selector(OwnerGoldCoinInfo::pollGoldCoins), .2f);
        return true;
    }
    void pollGoldCoins(float) {
        if (m_fields->drawn) return;
        int count = goldCoinCounts[m_fields->key];
        if (count < 0) return;
        m_fields->drawn = true;
        unschedule(schedule_selector(OwnerGoldCoinInfo::pollGoldCoins));
        if (count == 0) return;
        CCPoint position{getContentSize().width * .32f, getContentSize().height * .55f};
        CCNode* parent = this;
        if (m_coins && m_coins->count()) {
            auto* first = static_cast<CCNode*>(m_coins->objectAtIndex(0));
            position = first->getPosition();
            parent = first->getParent();
            for (auto* coin : CCArrayExt<CCNode*>(m_coins)) coin->setVisible(false);
        }
        for (int i = 0; i < count; ++i) {
            auto* coin = CCSprite::createWithSpriteFrameName("GJ_coinsIcon_001.png");
            if (!coin) continue;
            coin->setScale(13.f / coin->getContentSize().width);
            coin->setPosition(position + CCPoint{20.f * i, 0.f});
            parent->addChild(coin, 20);
        }
    }
};
