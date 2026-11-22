#include <Geode/Geode.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/utils/base64.hpp>
#include <Geode/cocos/support/zip_support/ZipUtils.h>
#include <Geode/utils/web.hpp>
#include <Geode/cocos/shaders/ccShaders.h>
#include <thread>
#include "../managers/SessionManager.hpp"
#include "../utils/Utils.hpp"

using namespace geode::prelude;

namespace {
void applyCoinColorShader(GameObject* coin) {
    if (coin->m_baseColor) coin->m_baseColor->m_defaultColorID = 1011;
    if (!coin->m_detailColor) coin->m_detailColor = new GJSpriteColor();
    coin->m_detailColor->m_defaultColorID = 1011;
    if (!coin->m_colorSprite) {
        coin->m_colorSprite = CCSprite::create();
        coin->addChild(coin->m_colorSprite);
    }
    if (!coin->getUserObject("owner-gold-color"_spr)) {
        auto* candidate = new CCGLProgram();
        char const* fragment = R"(
#ifdef GL_ES
precision mediump float;
#endif
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
uniform sampler2D CC_Texture0;
uniform vec3 u_base;
uniform vec3 u_detail;
uniform vec4 u_rect;
void main() {
    vec4 tex = texture2D(CC_Texture0, v_texCoord);
    vec2 localUV = (v_texCoord-u_rect.xy)/u_rect.zw-vec2(0.5);
    float rim = smoothstep(0.415, 0.44, length(localUV));
    vec3 tint = mix(u_detail, u_base, rim);
    float tinted = step(0.01, length(tint-vec3(1.0)));
    float brightness = max(tex.r, max(tex.g, tex.b));
    tex.rgb = mix(tex.rgb, vec3(brightness)*tint, tinted);
    gl_FragColor = vec4(tex.rgb, tex.a*v_fragmentColor.a);
})";
        if (!candidate->initWithVertexShaderByteArray(ccPositionTextureColor_vert, fragment)) {
            candidate->release();
            return;
        }
        candidate->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
        candidate->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
        candidate->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
        if (!candidate->link()) { candidate->release(); return; }
        candidate->updateUniforms();
        coin->setShaderProgram(candidate);
        coin->setUserObject("owner-gold-color"_spr, CCBool::create(true));
        candidate->release();
    }
}
void updateCoinColorShader(GameObject* coin) {
    if (!coin->getUserObject("owner-gold-color"_spr)) return;
    auto* program = coin->getShaderProgram();
    auto* texture = coin->getTexture();
    if (!program || !texture) return;
    auto base = coin->getActiveColorForMode(coin->getMainColorMode(), true);
    auto detail = coin->getActiveColorForMode(coin->getSecondaryColorMode(), false);
    auto rect = coin->getTextureRect();
    float width = texture->getContentSize().width;
    float height = texture->getContentSize().height;
    program->use();
    program->setUniformLocationWith3f(program->getUniformLocationForName("u_base"), base.r/255.f, base.g/255.f, base.b/255.f);
    program->setUniformLocationWith3f(program->getUniformLocationForName("u_detail"), detail.r/255.f, detail.g/255.f, detail.b/255.f);
    program->setUniformLocationWith4f(program->getUniformLocationForName("u_rect"),
        rect.origin.x/width, rect.origin.y/height, rect.size.width/width, rect.size.height/height);
}
bool ownerCoinEditor(GJBaseGameLayer* layer) {
    auto* editor = typeinfo_cast<LevelEditorLayer*>(layer);
    return editor && editor->m_level && Utils::isOnGdps()
        && SessionManager::accountID() == 1
        && static_cast<int>(editor->m_level->m_originalLevel) == 0;
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

class $modify(OwnerGoldCoinObject, GameObject) {
    struct Fields {
        int originalID = 0;
        GameObjectType originalType = static_cast<GameObjectType>(0);
        GameObjectType originalSavedType = static_cast<GameObjectType>(0);
    };

public:
    void refreshCoinColors(float) { updateCoinColorShader(this); }
    void startCoinColors() {
        applyCoinColorShader(this);
        schedule(schedule_selector(OwnerGoldCoinObject::refreshCoinColors), .03f);
        refreshCoinColors(0.f);
    }

    void restoreOwnerCoin(float) {
        if (!m_fields->originalID) return;
        m_objectID = m_fields->originalID;
        m_objectType = m_fields->originalType;
        m_savedObjectType = m_fields->originalSavedType;
        m_fields->originalID = 0;
        if (auto* coin = typeinfo_cast<EffectGameObject*>(this)) {
            if (PlayLayer::get() && coin->m_secretCoinID == 0) {
                setUserObject("owner-gold-no-reward"_spr, CCBool::create(true));
                coin->m_secretCoinID = 1;
            }
        }
        GameObject::customSetup();
        startCoinColors();
        log::info("Owner gold coin restored: visible={}", isVisible());
    }

    void playDestroyObjectAnim(GJBaseGameLayer* layer) {
        if (m_objectID == 142 && Utils::isOnGdps()) {
            if (auto* play = typeinfo_cast<PlayLayer*>(layer)) {
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
        bool playCoin = play && play->m_level && Utils::isOnGdps()
            && static_cast<int>(play->m_level->m_originalLevel) == 0;
        if (!object || object->m_objectID != 142 || (!ownerCoinEditor(layer) && !playCoin)) return object;
        auto* hooked = static_cast<OwnerGoldCoinObject*>(object);
        hooked->m_fields->originalID = object->m_objectID;
        hooked->m_fields->originalType = object->m_objectType;
        hooked->m_fields->originalSavedType = object->m_savedObjectType;
        object->m_objectID = 1329;
        object->m_objectType = GameObjectType::UserCoin;
        object->m_savedObjectType = GameObjectType::UserCoin;
        hooked->scheduleOnce(schedule_selector(OwnerGoldCoinObject::restoreOwnerCoin), 0.f);
        return object;
    }
};

class $modify(OwnerGoldCoinLevelEditor, LevelEditorLayer) {
    GameObject* createObject(int key, CCPoint position, bool noUndo) {
        auto* object = LevelEditorLayer::createObject(key, position, noUndo);
        if (object && key == 142 && ownerCoinEditor(this))
            static_cast<OwnerGoldCoinObject*>(object)->startCoinColors();
        return object;
    }
    bool init(GJGameLevel* level, bool noUI) {
        bool owner = level && Utils::isOnGdps() && SessionManager::accountID() == 1
            && static_cast<int>(level->m_originalLevel) == 0;
        auto originalType = level ? level->m_levelType : static_cast<GJLevelType>(0);
        auto originalLocal = level && level->m_localOrSaved;
        if (owner) {
            level->m_levelType = GJLevelType::Main;
            level->m_localOrSaved = true;
        }
        bool result = LevelEditorLayer::init(level, noUI);
        if (owner) {
            level->m_levelType = originalType;
            level->m_localOrSaved = originalLocal;
        }
        return result;
    }

    gd::string getLevelString() {
        auto result = LevelEditorLayer::getLevelString();
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

class $modify(OwnerGoldCoinPlayLayer, PlayLayer) {
    void resetLevel() {
        setUserObject("owner-gold-slots"_spr, nullptr);
        PlayLayer::resetLevel();
    }

    void levelComplete() {
        std::vector<int> collected;
        int levelID = 0;
        if (Utils::isOnGdps() && m_level && !m_isPracticeMode
            && m_level->m_levelType == GJLevelType::Saved) {
            levelID = static_cast<int>(m_level->m_levelID);
            if (auto* slots = typeinfo_cast<CCArray*>(getUserObject("owner-gold-slots"_spr)))
                for (auto* slot : CCArrayExt<CCInteger*>(slots)) collected.push_back(slot->getValue());
        }
        PlayLayer::levelComplete();
        if (levelID <= 0 || collected.empty()) return;
        auto* account = GJAccountManager::get();
        int accountID = static_cast<int>(account->m_accountID);
        std::string credential = account->m_GJP2;
        std::string url = Utils::serverBaseUrl() + "/goldcoin/claim.php";
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
                    if (SessionManager::accountID() == accountID && total >= 0)
                        GameStatsManager::get()->setStatIfHigher("8", total);
                });
            }
        }).detach();
    }

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        bool hasCoin = level && Utils::isOnGdps()
            && static_cast<int>(level->m_originalLevel) == 0
            && containsCoin(level->m_levelString);
        if (level && Utils::isOnGdps()) {
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
        if (hasCoin) {
            level->m_levelType = originalType;
            level->m_localOrSaved = originalLocal;
        }
        return result;
    }
};
