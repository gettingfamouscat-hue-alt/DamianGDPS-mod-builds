#include <Geode/Geode.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/utils/base64.hpp>
#include <Geode/cocos/support/zip_support/ZipUtils.h>
#include <Geode/utils/web.hpp>
#include <thread>
#include "../utils/CoinContext.hpp"
#include "../utils/GoldCoinProgress.hpp"

using namespace geode::prelude;

namespace {
thread_local GJBaseGameLayer* officialCoinLoadingLayer = nullptr;
struct OfficialCoinLoadScope {
    GJBaseGameLayer* previous;
    OfficialCoinLoadScope(bool enabled, GJBaseGameLayer* layer)
        : previous(officialCoinLoadingLayer) {
        if (enabled) officialCoinLoadingLayer = layer;
    }
    ~OfficialCoinLoadScope() { officialCoinLoadingLayer = previous; }
};
bool customLevel(GJGameLevel* level) {
    return level && (level->m_levelType == GJLevelType::Editor || level->m_levelType == GJLevelType::Saved);
}
bool officialCosmeticLayer(GJBaseGameLayer* layer) {
    return layer && !GoldenCoin::isOnGdps()
        && (officialCoinLoadingLayer == layer || customLevel(layer->m_level));
}
void setCosmeticCoin(GameObject* object) {
    object->setUserObject("owner-gold-cosmetic"_spr, CCBool::create(true));
    object->m_objectType = GameObjectType::Collectible;
    object->m_savedObjectType = GameObjectType::Collectible;
    if (auto* effect = typeinfo_cast<EffectGameObject*>(object)) effect->m_collectibleIsPickupItem = true;
    if (auto* frame = CCSpriteFrameCache::get()->spriteFrameByName("secretCoin_01_001.png"))
        object->setDisplayFrame(frame);
    object->setColor(ccWHITE);
}
bool ownerCoinEditor(GJBaseGameLayer* layer) {
    auto* editor = typeinfo_cast<LevelEditorLayer*>(layer);
    return editor && editor->m_level && (officialCosmeticLayer(editor)
        || (GoldenCoin::isOnGdps() && GoldenCoin::accountID() == 1
            && static_cast<int>(editor->m_level->m_originalLevel) == 0));
}
bool containsCoin(gd::string const& levelString) {
    auto text = std::string(levelString.c_str());
    if (text.starts_with("H4sI") && text.size() < 16 * 1024 * 1024) {
        auto binary = geode::utils::base64::decodeString(text);
        if (binary) {
            auto compressed = binary.unwrap();
            unsigned char* inflated = nullptr;
            int length = cocos2d::ZipUtils::ccInflateMemory(
                reinterpret_cast<unsigned char*>(compressed.data()),
                static_cast<unsigned int>(compressed.size()), &inflated);
            if (length > 0 && length < 32 * 1024 * 1024 && inflated)
                text.assign(reinterpret_cast<char*>(inflated), static_cast<size_t>(length));
            free(inflated);
        }
    }
    return text.find(";1,142,") != std::string::npos
        || text.starts_with("1,142,");
}
}

class $modify(OwnerGoldCoinSprite, CCSprite) {
    void setDisplayFrame(CCSpriteFrame* frame) {
        auto* object = typeinfo_cast<GameObject*>(this);
        if (frame && object && (GoldenCoin::isOnGdps() || object->getUserObject("owner-gold-cosmetic"_spr))
            && (object->m_objectID == 142 || object->getUserObject("owner-gold-object"_spr))) {
            for (int i = 1; i <= 4; ++i) {
                auto suffix = fmt::format("_00{}.png", i);
                auto* silver = CCSpriteFrameCache::get()->spriteFrameByName(("secretCoin_2_01" + suffix).c_str());
                if (silver && frame->getTexture() == silver->getTexture() && frame->getRect().equals(silver->getRect())) {
                    if (auto* gold = CCSpriteFrameCache::get()->spriteFrameByName(("secretCoin_01" + suffix).c_str())) frame = gold;
                    break;
                }
            }
        }
        CCSprite::setDisplayFrame(frame);
    }
};

class $modify(OwnerGoldCoinObject, GameObject) {
    struct Fields {
        int originalID = 0;
        GameObjectType originalType = static_cast<GameObjectType>(0);
        GameObjectType originalSavedType = static_cast<GameObjectType>(0);
    };

public:
    void customSetup() {
        GameObject::customSetup();
        if (getUserObject("owner-gold-cosmetic"_spr)) {
            setCosmeticCoin(this);
            return;
        }
        if (GoldenCoin::isOnGdps() && (m_objectID == 142 || m_fields->originalID == 142)) {
            if (auto* frame = CCSpriteFrameCache::get()->spriteFrameByName("secretCoin_01_001.png"))
                setDisplayFrame(frame);
            setColor(ccWHITE);
        }
    }
    void restoreOwnerCoin(float) {
        if (!m_fields->originalID) return;
        m_objectID = m_fields->originalID;
        m_objectType = m_fields->originalType;
        m_savedObjectType = m_fields->originalSavedType;
        m_fields->originalID = 0;
        if (auto* coin = typeinfo_cast<EffectGameObject*>(this)) {
            if (!getUserObject("owner-gold-cosmetic"_spr) && PlayLayer::get() && coin->m_secretCoinID == 0) {
                setUserObject("owner-gold-no-reward"_spr, CCBool::create(true));
                coin->m_secretCoinID = 1;
            }
        }
        GameObject::customSetup();
        if (getUserObject("owner-gold-cosmetic"_spr)) {
            setCosmeticCoin(this);
            return;
        }
        if (auto* frame = CCSpriteFrameCache::get()->spriteFrameByName("secretCoin_01_001.png"))
            setDisplayFrame(frame);
        setColor(ccWHITE);
        log::info("Owner gold coin restored: visible={}", isVisible());
    }

    void playDestroyObjectAnim(GJBaseGameLayer* layer) {
        if (m_objectID == 142 && GoldenCoin::isOnGdps()) {
            if (auto* play = typeinfo_cast<PlayLayer*>(layer)) {
                if (!play->m_isPracticeMode) {
                    auto* index = typeinfo_cast<CCInteger*>(getUserObject("owner-gold-index"_spr));
                    if (index) {
                        auto* touched = typeinfo_cast<CCArray*>(play->getUserObject("owner-gold-collected"_spr));
                        if (!touched) {
                            touched = CCArray::create();
                            play->setUserObject("owner-gold-collected"_spr, touched);
                        }
                        touched->addObject(CCInteger::create(index->getValue()));
                    }
                }
                if (auto* coin = typeinfo_cast<EffectGameObject*>(this)) {
                    if (!getUserObject("owner-gold-no-reward"_spr) && !play->m_isPracticeMode && coin->m_secretCoinID >= 1 && coin->m_secretCoinID <= 3) {
                        auto* slots = typeinfo_cast<CCArray*>(play->getUserObject("owner-gold-slots"_spr));
                        if (!slots) {
                            slots = CCArray::create();
                            play->setUserObject("owner-gold-slots"_spr, slots);
                        }
                        bool seen = false;
                        for (auto* value : CCArrayExt<CCInteger*>(slots))
                            if (value->getValue() == coin->m_secretCoinID) seen = true;
                        if (!seen) slots->addObject(CCInteger::create(coin->m_secretCoinID));
                    }
                }
            }
        }
        GameObject::playDestroyObjectAnim(layer);
    }

    static GameObject* objectFromVector(gd::vector<gd::string>& values,
        gd::vector<void*>& present, GJBaseGameLayer* layer, bool lowDetail) {
        auto* object = GameObject::objectFromVector(values, present, layer, lowDetail);
        auto* play = typeinfo_cast<PlayLayer*>(layer);
        bool playCoin = play && play->m_level && GoldenCoin::isOnGdps()
            && static_cast<int>(play->m_level->m_originalLevel) == 0;
        bool cosmetic = officialCosmeticLayer(layer);
        if (!object || object->m_objectID != 142 || (!ownerCoinEditor(layer) && !playCoin && !cosmetic)) return object;
        auto* hooked = static_cast<OwnerGoldCoinObject*>(object);
        if (cosmetic) object->setUserObject("owner-gold-cosmetic"_spr, CCBool::create(true));
        if (playCoin) {
            auto* oldIndex = typeinfo_cast<CCInteger*>(play->getUserObject("owner-gold-count"_spr));
            int index = oldIndex ? oldIndex->getValue() + 1 : 1;
            play->setUserObject("owner-gold-count"_spr, CCInteger::create(index));
            object->setUserObject("owner-gold-index"_spr, CCInteger::create(index));
            if (auto* coin = typeinfo_cast<EffectGameObject*>(object)) {
                if (coin->m_secretCoinID == 0 && index >= 1 && index <= 3) {
                    object->setUserObject("owner-gold-no-reward"_spr, CCBool::create(true));
                    coin->m_secretCoinID = index;
                }
            }
        }
        object->setUserObject("owner-gold-object"_spr, CCBool::create(true));
        hooked->m_fields->originalID = object->m_objectID;
        hooked->m_fields->originalType = object->m_objectType;
        hooked->m_fields->originalSavedType = object->m_savedObjectType;
        object->m_objectID = cosmetic ? 1275 : 1329;
        object->m_objectType = cosmetic ? GameObjectType::Collectible : GameObjectType::UserCoin;
        object->m_savedObjectType = object->m_objectType;
        hooked->scheduleOnce(schedule_selector(OwnerGoldCoinObject::restoreOwnerCoin), 0.f);
        return object;
    }
};

class $modify(OwnerGoldCoinLevelEditor, LevelEditorLayer) {
    GameObject* createObject(int key, CCPoint position, bool noUndo) {
        auto* object = LevelEditorLayer::createObject(key, position, noUndo);
        if (object && key == 142 && officialCosmeticLayer(this)) setCosmeticCoin(object);
        return object;
    }
    bool init(GJGameLevel* level, bool noUI) {
        bool owner = level && GoldenCoin::isOnGdps() && GoldenCoin::accountID() == 1
            && static_cast<int>(level->m_originalLevel) == 0;
        bool cosmetic = !GoldenCoin::isOnGdps() && customLevel(level) && containsCoin(level->m_levelString);
        OfficialCoinLoadScope scope(cosmetic, this);
        auto originalType = level ? level->m_levelType : static_cast<GJLevelType>(0);
        auto originalLocal = level && level->m_localOrSaved;
        if (owner || cosmetic) {
            level->m_levelType = GJLevelType::Main;
            level->m_localOrSaved = true;
        }
        bool result = LevelEditorLayer::init(level, noUI);
        if (owner || cosmetic) {
            level->m_levelType = originalType;
            level->m_localOrSaved = originalLocal;
        }
        return result;
    }

    gd::string getLevelString() {
        auto result = LevelEditorLayer::getLevelString();
        if (officialCosmeticLayer(this) && m_level) {
            int userCoins = 0;
            if (auto* objects = getAllObjects())
                for (auto* object : CCArrayExt<GameObject*>(objects))
                    if (object && object->m_objectID == 1329) ++userCoins;
            // Cosmetic gold coins must not be advertised as user-coin rewards.
            m_level->m_coins = std::min(userCoins, 3);
        }
        if (ownerCoinEditor(this)) {
            int placed = 0;
            if (auto* objects = getAllObjects())
                for (auto* object : CCArrayExt<GameObject*>(objects))
                    if (object && object->m_objectID == 142) ++placed;
            log::info("Owner gold coin save: {} placed, serialized={}", placed, containsCoin(result));
        }
        return result;
    }
};

class $modify(OfficialCosmeticCoinPickup, GJBaseGameLayer) {
    void pickupItem(EffectGameObject* object) {
        if (object && object->getUserObject("owner-gold-cosmetic"_spr) && officialCosmeticLayer(this)) {
            if (object->m_isDisabled || object->m_isDisabled2) return;
            auto* stats = GameStatsManager::get();
            int secretCoins = stats->getStat("8");
            int userCoins = stats->getStat("12");
            auto type = object->m_objectType;
            auto slot = object->m_secretCoinID;
            if (slot < 1 || slot > 3) object->m_secretCoinID = 1;
            object->m_objectType = GameObjectType::SecretCoin;
            object->playDestroyObjectAnim(this);
            object->m_objectType = type;
            object->m_secretCoinID = slot;
            object->m_isActivated = true;
            object->destroyObject();
            log::info("Cosmetic gold coin pickup: secret total unchanged={}, user total unchanged={}",
                stats->getStat("8") == secretCoins, stats->getStat("12") == userCoins);
            // Skip native pickup accounting: no coins, items, counters, or rewards.
            return;
        }
        GJBaseGameLayer::pickupItem(object);
    }
};

class $modify(OwnerGoldCoinPlayLayer, PlayLayer) {
    void resetLevel() {
        setUserObject("owner-gold-slots"_spr, nullptr);
        setUserObject("owner-gold-collected"_spr, nullptr);
        PlayLayer::resetLevel();
    }

    void levelComplete() {
        std::vector<int> collected;
        std::vector<int> displayCollected;
        int levelID = 0;
        if (GoldenCoin::isOnGdps() && m_level && !m_isPracticeMode
            && m_level->m_levelType == GJLevelType::Saved) {
            levelID = static_cast<int>(m_level->m_levelID);
            if (auto* slots = typeinfo_cast<CCArray*>(getUserObject("owner-gold-slots"_spr)))
                for (auto* slot : CCArrayExt<CCInteger*>(slots)) collected.push_back(slot->getValue());
            if (auto* touched = typeinfo_cast<CCArray*>(getUserObject("owner-gold-collected"_spr)))
                for (auto* index : CCArrayExt<CCInteger*>(touched)) displayCollected.push_back(index->getValue());
        }
        PlayLayer::levelComplete();
        if (levelID > 0) for (int index : displayCollected)
            if (index >= 1 && index <= 3) Mod::get()->setSavedValue(GoldCoinProgress::key(levelID, index), true);
        if (levelID <= 0 || collected.empty()) return;
        auto* account = GJAccountManager::get();
        int accountID = static_cast<int>(account->m_accountID);
        std::string credential = account->m_GJP2;
        std::string url = GoldenCoin::serverBaseUrl() + "/goldcoin/claim.php";
        if (accountID <= 0 || credential.empty()) return;
        std::thread([levelID, accountID, credential, url, collected] {
            for (int slot : collected) {
                auto request = web::WebRequest();
                request.header("Content-Type", "application/x-www-form-urlencoded");
                request.bodyString(fmt::format("accountID={}&gjp2={}&levelID={}&slot={}",
                    accountID, credential, levelID, slot));
                request.timeout(std::chrono::seconds(8));
                auto response = request.postSync(url);
                auto parsed = response.json();
                if (!response.ok() || !parsed) continue;
                auto body = parsed.unwrap();
                if (!body["ok"].asBool().unwrapOr(false)) continue;
                int total = body["coins"].asInt().unwrapOr(-1);
                Loader::get()->queueInMainThread([accountID, total] {
                    if (GoldenCoin::accountID() == accountID && total >= 0)
                        GameStatsManager::get()->setStatIfHigher("8", total);
                });
            }
        }).detach();
    }

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        bool gdpsCoin = level && GoldenCoin::isOnGdps()
            && static_cast<int>(level->m_originalLevel) == 0
            && containsCoin(level->m_levelString);
        bool cosmetic = !GoldenCoin::isOnGdps() && customLevel(level) && containsCoin(level->m_levelString);
        bool hasCoin = gdpsCoin || cosmetic;
        OfficialCoinLoadScope scope(cosmetic, this);
        if (level && GoldenCoin::isOnGdps()) {
            std::string data = level->m_levelString.c_str();
            log::info("Owner gold coin play load: hasCoin={}, bytes={}, prefix={}",
                hasCoin, data.size(), data.substr(0, std::min<size_t>(16, data.size())));
        }
        auto originalType = level ? level->m_levelType : static_cast<GJLevelType>(0);
        auto originalLocal = level && level->m_localOrSaved;
        if (hasCoin) {
            level->m_levelType = GJLevelType::Main;
            level->m_localOrSaved = true;
        }
        bool result = PlayLayer::init(level, useReplay, dontCreateObjects);
        if (result && gdpsCoin) setUserObject("owner-gold-level"_spr, CCBool::create(true));
        if (hasCoin) {
            level->m_levelType = originalType;
            level->m_localOrSaved = originalLocal;
        }
        return result;
    }
};
