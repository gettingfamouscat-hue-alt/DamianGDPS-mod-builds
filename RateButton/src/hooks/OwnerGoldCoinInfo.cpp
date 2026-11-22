#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/utils/web.hpp>
#include <thread>
#include <map>
#include "../utils/Utils.hpp"
using namespace geode::prelude;
namespace {
std::map<std::string, int> goldCoinCounts;
}
class $modify(OwnerGoldCoinInfo, LevelInfoLayer) {
    struct Fields { std::string key; bool drawn = false; };
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;
        if (!Utils::isOnGdps() || !level || static_cast<int>(level->m_levelID) <= 0) return true;
        m_fields->key = Utils::serverBaseUrl() + "/goldcoin/status.php?levelID="
            + std::to_string(static_cast<int>(level->m_levelID));
        auto key = m_fields->key;
        goldCoinCounts[key] = -1;
        std::thread([key] {
            auto request = web::WebRequest();
            request.timeout(std::chrono::seconds(8));
            auto response = request.getSync(key);
            auto parsed = response.json();
            int count = response.ok() && parsed ? parsed.unwrap()["count"].asInt().unwrapOr(0) : 0;
            Loader::get()->queueInMainThread([key, count] { goldCoinCounts[key] = count; });
        }).detach();
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
            auto* coin = CCSprite::createWithSpriteFrameName("secretCoinUI_001.png");
            if (!coin) continue;
            coin->setScale(12.f / coin->getContentSize().width);
            coin->setPosition(position + CCPoint{20.f * i, 0.f});
            parent->addChild(coin, 20);
        }
    }
};
