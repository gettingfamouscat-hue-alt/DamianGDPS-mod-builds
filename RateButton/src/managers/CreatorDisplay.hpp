#pragma once
#include <Geode/Geode.hpp>
#include <functional>
namespace CreatorDisplay {
struct Value { std::string name, color="00FF00"; bool clickable=false; int badges=0; };
struct State {
    geode::Ref<cocos2d::CCLabelBMFont> label;
    geode::Ref<cocos2d::CCMenuItem> button;
    geode::Ref<cocos2d::CCSprite> plusBadge, creatorBadge;
    std::string original;
    cocos2d::ccColor3B color{};
    bool enabled=true, applied=false;
    cocos2d::CCPoint position{};
    float width=0.f;
    void restore();
};
Value cached(int id);
void apply(cocos2d::CCNode* root, int id, State& state, bool leftAlign = true);
void save(int id, Value value, std::function<void(bool,std::string)> done);
}
