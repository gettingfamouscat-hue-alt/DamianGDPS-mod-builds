#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/utils/web.hpp>
#include <thread>
#include <map>
#include "../utils/Utils.hpp"
#include "../utils/GoldCoinProgress.hpp"
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
    if (node->getID() == "owner-gold-row"_spr) return;
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
    char const* getEndText() {
        auto* play = PlayLayer::get();
        // The native no-coin message shares the coin row's position. Suppress
        // it at creation for every gold-coin level, regardless of its wording.
        if (Utils::isOnGdps() && play && play->getUserObject("owner-gold-level"_spr)) return "";
        return EndLevelLayer::getEndText();
    }
    void customSetup() {
        auto* play = PlayLayer::get();
        bool gold = Utils::isOnGdps() && play && play->m_level && play->getUserObject("owner-gold-level"_spr);
        EndLevelLayer::customSetup();
        if (!gold) return;
        auto* countValue = typeinfo_cast<CCInteger*>(play->getUserObject("owner-gold-count"_spr));
        int count = std::clamp(countValue ? countValue->getValue() : play->m_level->m_coins, 0, 3);
        if (!count) return;
        replaceCoinArt(this, true);
        bool collected[4] = {};
        if (auto* touched = typeinfo_cast<CCArray*>(play->getUserObject("owner-gold-collected"_spr))) {
            for (auto* value : CCArrayExt<CCInteger*>(touched)) {
                int index = value->getValue();
                if (index >= 1 && index <= 3) collected[index] = true;
            }
        }
        auto* row = CCNode::create();
        row->setID("owner-gold-row"_spr);
        auto size = CCDirector::get()->getWinSize();
        row->setPosition({size.width / 2.f, size.height * .31f});
        m_mainLayer->addChild(row, 30);
        for (int index = 1; index <= count; ++index) {
            auto* coin = CCSprite::createWithSpriteFrameName("GJ_coinsIcon_001.png");
            if (!coin) continue;
            float scale = 32.f / coin->getContentSize().width;
            coin->setScale(scale);
            coin->setColor(collected[index] ? ccWHITE : ccColor3B{90,90,90});
            coin->setPosition({(index - 1 - (count - 1) / 2.f) * 30.f, 0.f});
            row->addChild(coin);
            if (collected[index]) {
                coin->setScale(scale * .65f);
                coin->runAction(CCSequence::create(CCDelayTime::create(.12f * index),
                    CCEaseBackOut::create(CCScaleTo::create(.25f, scale)), nullptr));
            }
        }
        schedule(schedule_selector(OwnerGoldCoinEndScreen::refreshEndCoins), .05f);
    }
    void refreshEndCoins(float) { replaceCoinArt(this, true); }
};
}
class $modify(OwnerGoldCoinCell, LevelCell) {
    struct Fields { std::string key; bool logged = false; Ref<CCNode> row; int count = 0; };
    void loadFromLevel(GJGameLevel* level) {
        unschedule(schedule_selector(OwnerGoldCoinCell::refreshGoldArt));
        if (m_fields->row) m_fields->row->removeFromParent();
        m_fields->row = nullptr;
        m_fields->count = 0;
        m_fields->key.clear();
        m_fields->logged = false;
        LevelCell::loadFromLevel(level);
        // Local editor IDs can coincide with published level IDs. Only online
        // levels should look up and display the server's gold-coin metadata.
        if (!Utils::isOnGdps() || !level || level->m_levelType != GJLevelType::Saved
            || static_cast<int>(level->m_levelID) <= 0) return;
        m_fields->key = requestGoldCoins(static_cast<int>(level->m_levelID));
        if (goldCoinCounts[m_fields->key] < 0) replaceCoinArt(this, true);
        else if (goldCoinCounts[m_fields->key] > 0) replaceCoinArt(this, true);
        refreshGoldArt(0.f);
        schedule(schedule_selector(OwnerGoldCoinCell::refreshGoldArt), .05f);
    }
    void refreshGoldArt(float) {
        if (m_fields->key.empty() || !m_level || m_level->m_levelType != GJLevelType::Saved) return;
        if (!m_fields->logged && goldCoinCounts[m_fields->key] >= 0) {
            log::info("Gold coin list status: {} = {}", m_fields->key, goldCoinCounts[m_fields->key]);
            m_fields->logged = true;
        }
        int count = goldCoinCounts[m_fields->key];
        if (count == 0) restorePendingCoinArt(this);
        if (count <= 0 || !m_level) return;
        replaceCoinArt(this, true);
        if (!m_fields->row || m_fields->count != count) {
            if (m_fields->row) m_fields->row->removeFromParent();
            auto* row = CCNode::create();
            row->setID("owner-gold-row"_spr);
            m_fields->row = row;
            m_fields->count = count;
            addChild(row, 50);
            for (int i = 0; i < count; ++i) {
                auto* coin = CCSprite::createWithSpriteFrameName("GJ_coinsIcon_001.png");
                coin->setScale(12.f / coin->getContentSize().width);
                coin->setPosition({(i - (count - 1) / 2.f) * 10.f, 0.f});
                row->addChild(coin, 0, i + 1);
            }
        }
        if (auto* face = getChildByIDRecursive("difficulty-sprite")) {
            auto size = face->getContentSize();
            auto center = convertToNodeSpace(face->convertToWorldSpace({size.width / 2.f, 0.f}));
            m_fields->row->setPosition(center + CCPoint{0.f, -8.f});
        }
        for (int i = 1; i <= count; ++i) {
            auto* coin = static_cast<CCSprite*>(m_fields->row->getChildByTag(i));
            coin->setColor(GoldCoinProgress::collected(m_level, i) ? ccWHITE : ccColor3B{105,105,105});
        }
    }
};
class $modify(OwnerGoldCoinInfo, LevelInfoLayer) {
    struct Fields { std::string key; bool drawn = false; };
    void refreshGoldProgress() {
        if (!m_level || !Utils::isOnGdps()) return;
        for (int index = 1; index <= 3; ++index) {
            auto id = fmt::format("owner-gold-icon-{}", index);
            if (auto* coin = typeinfo_cast<CCSprite*>(getChildByIDRecursive(id)))
                coin->setColor(GoldCoinProgress::collected(m_level, index) ? ccWHITE : ccColor3B{105,105,105});
        }
    }
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;
        if (!Utils::isOnGdps() || !level || static_cast<int>(level->m_levelID) <= 0) return true;
        m_fields->key = requestGoldCoins(static_cast<int>(level->m_levelID));
        schedule(schedule_selector(OwnerGoldCoinInfo::pollGoldCoins), .2f);
        return true;
    }
    void pollGoldCoins(float) {
        if (m_fields->drawn) { refreshGoldProgress(); return; }
        int count = goldCoinCounts[m_fields->key];
        if (count < 0) return;
        m_fields->drawn = true;
        if (count == 0) { unschedule(schedule_selector(OwnerGoldCoinInfo::pollGoldCoins)); return; }
        CCPoint position{getContentSize().width * .32f, getContentSize().height * .55f};
        CCNode* parent = this;
        if (m_coins && m_coins->count()) {
            auto* first = static_cast<CCNode*>(m_coins->objectAtIndex(0));
            position = first->getPosition();
            parent = first->getParent();
            for (auto* coin : CCArrayExt<CCNode*>(m_coins)) coin->setVisible(false);
        }
        if (auto* face = getChildByIDRecursive("difficulty-sprite")) {
            auto size = face->getContentSize();
            position.x = parent->convertToNodeSpace(face->convertToWorldSpace({size.width / 2.f, size.height / 2.f})).x;
        } else if (m_coins && m_coins->count()) {
            float total = 0.f;
            for (auto* coin : CCArrayExt<CCNode*>(m_coins)) total += coin->getPositionX();
            position.x = total / m_coins->count();
        }
        position.y += 9.f;
        for (int i = 0; i < count; ++i) {
            auto* coin = CCSprite::createWithSpriteFrameName("GJ_coinsIcon_001.png");
            if (!coin) continue;
            coin->setID(fmt::format("owner-gold-icon-{}", i + 1));
            coin->setScale(13.f / coin->getContentSize().width);
            coin->setPosition(position + CCPoint{11.f * (i - (count - 1) / 2.f), 0.f});
            coin->setColor(GoldCoinProgress::collected(m_level, i + 1) ? ccWHITE : ccColor3B{105,105,105});
            parent->addChild(coin, 20);
        }
    }
};
