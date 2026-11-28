#include <Geode/Geode.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <Geode/modify/CCKeypadDispatcher.hpp>
using namespace geode::prelude;
namespace {
bool blocked(CCNode* node) {
    if (!node || !node->isVisible() || !node->isRunning()) return false;
    if (typeinfo_cast<FLAlertLayer*>(node)) return true;
    if (auto* input=typeinfo_cast<CCTextInputNode*>(node); input && input->m_selected) return true;
    if (auto* children=node->getChildren()) for (auto* child:CCArrayExt<CCNode*>(children)) if (blocked(child)) return true;
    return false;
}
CCLayer* rankedLayer() {
    if (!Loader::get()->isModLoaded("tipp7.versus")) return nullptr;
    auto* scene=CCDirector::sharedDirector()->getRunningScene();
    if (!scene || blocked(scene)) return nullptr;
    for (auto const* id:{"tipp7.versus/ranked-season2-scene","tipp7.versus/ranked-scene"})
        if (auto* layer=typeinfo_cast<CCLayer*>(scene->getChildByIDRecursive(id)); layer && layer->isRunning() && layer->isVisible()) return layer;
    return nullptr;
}
void back(CCLayer* layer) {
    // Call the same virtual handler as Ranked's arrow. It retains leave-match
    // confirmations and cancellation rules; never pop the scene unconditionally.
    Ref<CCLayer> keep(layer);
    Loader::get()->queueInMainThread([keep] { if (keep->isRunning()) keep->keyBackClicked(); });
}
}
class $modify(VersusBackKeyboard,CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key,bool down,bool repeat,double timestamp) {
        if (key==KEY_Escape && down) if (auto* layer=rankedLayer()) {
            if (!repeat) back(layer);
            return true;
        }
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key,down,repeat,timestamp);
    }
};
class $modify(VersusBackKeypad,CCKeypadDispatcher) {
    bool dispatchKeypadMSG(ccKeypadMSGType type) {
        if (type==kTypeBackClicked) if (auto* layer=rankedLayer()) { back(layer); return true; }
        return CCKeypadDispatcher::dispatchKeypadMSG(type);
    }
};
