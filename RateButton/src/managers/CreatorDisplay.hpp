#pragma once
#include <Geode/Geode.hpp>
#include <functional>
namespace CreatorDisplay {
struct Value { std::string name, color="00FF00"; bool clickable=false; };
struct State {
    geode::Ref<cocos2d::CCLabelBMFont> label;
    geode::Ref<cocos2d::CCMenuItem> button;
    std::string original;
    cocos2d::ccColor3B color{};
    bool enabled=true, applied=false;
    void restore();
};
Value cached(int id);
void apply(cocos2d::CCNode* root, int id, State& state);
void save(int id, Value value, std::function<void(bool,std::string)> done);
}
