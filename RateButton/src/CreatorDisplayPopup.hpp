#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/TextInput.hpp>
#include "managers/CreatorDisplay.hpp"
using namespace geode::prelude;
class CreatorDisplayPopup : public Popup {
 int levelID=0; TextInput* name=nullptr; TextInput* color=nullptr;
 CCMenuItemToggler* clickable=nullptr; CCLabelBMFont* preview=nullptr;
 void updatePreview(float=0) {
  if(!preview)return;
  auto value=std::string(name->getString()); if(value.empty())value="Your name";
  preview->setString(value.c_str());
  auto code=std::string(color->getString());
  auto parse=[](std::string h)->ccColor3B {auto v=std::stoul(h,nullptr,16);return {static_cast<GLubyte>(v>>16),static_cast<GLubyte>(v>>8),static_cast<GLubyte>(v)};};
  auto valid=[](std::string h){return h.size()==6&&h.find_first_not_of("0123456789abcdefABCDEF")==std::string::npos;};
  if(valid(code))preview->setColor(parse(code));
  else if(code.size()==13&&code[6]=='-'&&valid(code.substr(0,6))&&valid(code.substr(7,6))) {
   preview->setColor({255,255,255});auto a=parse(code.substr(0,6)),b=parse(code.substr(7,6));
   int i=0;for(auto* n:CCArrayExt<CCNode*>(preview->getChildren()))if(auto* g=typeinfo_cast<CCSprite*>(n)) {
    float t=value.size()>1?static_cast<float>(i)/static_cast<float>(value.size()-1):0.f;
    g->setColor({static_cast<GLubyte>(a.r+(b.r-a.r)*t),static_cast<GLubyte>(a.g+(b.g-a.g)*t),static_cast<GLubyte>(a.b+(b.b-a.b)*t)});++i;
   }
  }
 }
 bool init(int id) {
  if(!Popup::init(390.f,330.f))return false;levelID=id;setTitle("Creator Display");auto v=CreatorDisplay::cached(id);
  auto text=[this](char const* s,float y){auto* l=CCLabelBMFont::create(s,"bigFont.fnt");l->setScale(.33f);l->setPosition({195,y});m_mainLayer->addChild(l);};
  text("Preview name (blank restores original)",281);
  name=TextInput::create(290,"Creator name");name->setFilter("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 _-");name->setString(v.name);name->setPosition({195,252});m_mainLayer->addChild(name);
  text("Hex color (RRGGBB or RRGGBB-RRGGBB)",220);
  color=TextInput::create(230,"00FF00");color->setFilter("0123456789abcdefABCDEF-");color->setString(v.color);color->setPosition({195,192});m_mainLayer->addChild(color);
  text("Color library - tap to use",161);
  auto add=[this](char const* label,char const* code,float x,int tag){
   auto* spr=ButtonSprite::create(label);spr->setScale(.42f);
   auto* button=CCMenuItemSpriteExtra::create(spr,this,menu_selector(CreatorDisplayPopup::onPreset));
   button->setTag(tag);button->setPosition({x,127});m_buttonMenu->addChild(button);
   auto* hex=CCLabelBMFont::create(code,"chatFont.fnt");hex->setScale(.45f);hex->setPosition({x,109});m_mainLayer->addChild(hex);
  };
  add("Purple > Red","#8A2BE2 > #FF3030",78,0);
  add("Yellow","#FFFF00",195,1);add("Green","#00FF00",310,2);
  preview=CCLabelBMFont::create("Your name","bigFont.fnt");preview->setScale(.55f);preview->setPosition({195,83});m_mainLayer->addChild(preview);
  clickable=CCMenuItemToggler::createWithStandardSprites(this,menu_selector(CreatorDisplayPopup::onToggle),.6f);
  clickable->toggle(v.clickable);clickable->setPosition({105,54});m_buttonMenu->addChild(clickable);
  text("Clickable profile",54);
  auto* save=CCMenuItemSpriteExtra::create(ButtonSprite::create("Save"),this,menu_selector(CreatorDisplayPopup::onSave));save->setScale(.75f);save->setPosition({195,24});m_buttonMenu->addChild(save);
  schedule(schedule_selector(CreatorDisplayPopup::updatePreview),.15f);updatePreview();return true;
 }
 void onToggle(CCObject*){}
 void onPreset(CCObject* sender){switch(static_cast<CCNode*>(sender)->getTag()){
  case 0:color->setString("8A2BE2-FF3030");break;
  case 1:color->setString("FFFF00");break;
  default:color->setString("00FF00");break;
 }updatePreview();}
 void onSave(CCObject*) {
  CreatorDisplay::Value v{std::string(name->getString()),std::string(color->getString()),clickable->isToggled()};
  CreatorDisplay::save(levelID,v,[](bool ok,std::string message){FLAlertLayer::create(ok?"Saved":"Save failed",message.c_str(),"OK")->show();});onClose(nullptr);
 }
public:
 static CreatorDisplayPopup* create(int id){auto* p=new CreatorDisplayPopup();if(p->init(id)){p->autorelease();return p;}delete p;return nullptr;}
};
