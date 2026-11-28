#pragma once
#include <Geode/Geode.hpp>
#include <functional>
namespace CreatorDisplay {
inline void trimBadge(cocos2d::CCSprite* sprite, bool plus) {
    if (!sprite) return;
    // The generated source textures contain stray opaque pixels outside the badge.
    // Show only the central connected badge, preserving its original artwork.
    auto rect = sprite->getTextureRect();
    if (plus) sprite->setTextureRect(cocos2d::CCRectMake(
        rect.origin.x + rect.size.width * 280.f / 1293.f,
        rect.origin.y + rect.size.height * 237.f / 1216.f,
        rect.size.width * 733.f / 1293.f,
        rect.size.height * 747.f / 1216.f));
    else sprite->setTextureRect(cocos2d::CCRectMake(
        rect.origin.x + rect.size.width * 283.f / 1265.f,
        rect.origin.y + rect.size.height * 300.f / 1243.f,
        rect.size.width * 692.f / 1265.f,
        rect.size.height * 676.f / 1243.f));
}
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
