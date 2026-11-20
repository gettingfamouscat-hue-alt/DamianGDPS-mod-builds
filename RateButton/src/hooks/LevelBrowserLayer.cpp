#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/binding/GJAccountManager.hpp>
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/utils/web.hpp>
#include "../utils/Utils.hpp"
#include <thread>
#include <chrono>
using namespace geode::prelude;
class $modify(MyLevelsReloadBrowser, LevelBrowserLayer) {
    struct Fields { bool loading=false; bool exited=false; };
    void onExit(){m_fields->exited=true;LevelBrowserLayer::onExit();}
    void acknowledge(int importID,int account,std::string credential,std::string url){
        std::thread([importID,account,credential,url](){
            auto r=web::WebRequest();r.header("Content-Type","application/x-www-form-urlencoded");
            r.bodyString("accountID="+std::to_string(account)+"&gjp2="+credential+"&delivered="+std::to_string(importID));
            r.timeout(std::chrono::seconds(8));r.postSync(url);
        }).detach();
    }

    bool init(GJSearchObject* object) {
        if(!LevelBrowserLayer::init(object))return false;
        if(std::string(getSearchTitle()).find("My Levels")==std::string::npos)return true;
        auto win=CCDirector::sharedDirector()->getWinSize();
        auto* menu=CCMenu::create();menu->setPosition({0,0});
        menu->setID("my-levels-reload-menu"_spr);addChild(menu,100);
        auto* sprite=CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
        if(!sprite)sprite=CCSprite::createWithSpriteFrameName("GJ_resetBtn_001.png");
        if(!sprite)return true;
        auto* button=CCMenuItemSpriteExtra::create(sprite,this,menu_selector(MyLevelsReloadBrowser::onReloadMine));
        button->setID("my-levels-reload"_spr);
        button->setScale(.8f);button->setPosition({32.f,win.height-105.f});menu->addChild(button);
        return true;
    }
    void onReloadMine(CCObject*) {
        if(std::string(getSearchTitle()).find("My Levels")==std::string::npos)return;
        if(m_fields->loading)return;
        auto* am=GJAccountManager::sharedState();
        if(!Utils::isOnGdps()||!am||am->m_accountID<=0){onRefresh(nullptr);return;}
        m_fields->loading=true;
        int account=am->m_accountID;
        std::string credential=am->m_GJP2;
        std::string url=Utils::serverBaseUrl()+"/tools/editorImports.php";
        geode::Ref<MyLevelsReloadBrowser> self=this;
        std::thread([self,account,credential,url](){
            auto request=web::WebRequest();request.header("Content-Type","application/x-www-form-urlencoded");
            request.bodyString("accountID="+std::to_string(account)+"&gjp2="+credential);
            request.timeout(std::chrono::seconds(15));auto response=request.postSync(url);
            Loader::get()->queueInMainThread([self,account,credential,url,response=std::move(response)]() mutable {
                self->m_fields->loading=false;
                if(self->m_fields->exited)return;
                auto* current=GJAccountManager::sharedState();
                if(!current||current->m_accountID!=account||std::string(current->m_GJP2)!=credential)return;
                auto data=response.json();
                if(!response.ok()||!data){Notification::create("Could not check editor imports",NotificationIcon::Error)->show();return;}
                auto body=data.unwrap();if(!body.isObject()||!body["ok"].asBool().unwrapOr(false))return;
                auto level=body["level"];
                if(level.isObject()){
                    auto* manager=GameLevelManager::sharedState();
                    auto* copy=manager?manager->createNewLevel():nullptr;
                    if(!copy){Notification::create("Could not create editor level",NotificationIcon::Error)->show();return;}
                    copy->m_levelName=level["levelName"].asString().unwrapOr("Copied Level");
                    copy->m_levelString=level["levelString"].asString().unwrapOr("");
                    copy->m_audioTrack=static_cast<int>(level["audioTrack"].asInt().unwrapOr(0));
                    copy->m_songID=static_cast<int>(level["songID"].asInt().unwrapOr(0));
                    copy->m_levelLength=static_cast<int>(level["levelLength"].asInt().unwrapOr(0));
                    if(copy->m_levelString.empty())return;
                    manager->saveLevel(copy);
                    self->acknowledge(static_cast<int>(level["importID"].asInt().unwrapOr(0)),account,credential,url);
                    Notification::create("Copied level added to My Levels",NotificationIcon::Success)->show();
                }
                self->onRefresh(nullptr);
            });
        }).detach();
    }
};
