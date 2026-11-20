#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/TextInput.hpp>
#include "managers/CreatorDisplay.hpp"
using namespace geode::prelude;
class CreatorDisplayPopup : public Popup {
 int levelID=0; TextInput* name=nullptr;TextInput* color=nullptr;CCMenuItemToggler* clickable=nullptr;
 bool init(int id) {
  if(!Popup::init(340.f,260.f))return false;levelID=id;setTitle("Creator Display");
  auto v=CreatorDisplay::cached(id);
  auto text=[this](char const* s,float y){auto* l=CCLabelBMFont::create(s,"bigFont.fnt");l->setScale(.35f);l->setPosition({170,y});m_mainLayer->addChild(l);};
  text("Preview name (blank restores original)",212);
  name=TextInput::create(280,"Creator name");name->setFilter("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 _-");name->setString(v.name);name->setPosition({170,184});m_mainLayer->addChild(name);
  text("Color (hex, green = 00FF00)",153);
  color=TextInput::create(180,"00FF00");color->setFilter("0123456789abcdefABCDEF");color->setString(v.color);color->setPosition({170,127});m_mainLayer->addChild(color);
  clickable=CCMenuItemToggler::createWithStandardSprites(this,menu_selector(CreatorDisplayPopup::onToggle),.6f);
  clickable->toggle(v.clickable);clickable->setPosition({65,88});m_buttonMenu->addChild(clickable);
  auto* l=CCLabelBMFont::create("Clickable profile","bigFont.fnt");l->setScale(.4f);l->setPosition({185,88});m_mainLayer->addChild(l);
  auto* b=CCMenuItemSpriteExtra::create(ButtonSprite::create("Save"),this,menu_selector(CreatorDisplayPopup::onSave));b->setPosition({170,40});m_buttonMenu->addChild(b);
  return true;
 }
 void onToggle(CCObject*){}
 void onSave(CCObject*) {
  CreatorDisplay::Value v{std::string(name->getString()),std::string(color->getString()),clickable->isToggled()};
  CreatorDisplay::save(levelID,v,[](bool ok,std::string message){FLAlertLayer::create(ok?"Saved":"Save failed",message.c_str(),"OK")->show();});
  onClose(nullptr);
 }
public:
 static CreatorDisplayPopup* create(int id){auto* p=new CreatorDisplayPopup();if(p->init(id)){p->autorelease();return p;}delete p;return nullptr;}
};
