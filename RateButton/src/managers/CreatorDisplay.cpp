#include "CreatorDisplay.hpp"
#include "SessionManager.hpp"
#include "../utils/Utils.hpp"
#include <Geode/binding/GJAccountManager.hpp>
#include <Geode/utils/web.hpp>
#include <chrono>
#include <thread>
#include <map>
#include <algorithm>
using namespace geode::prelude;
namespace CreatorDisplay {
namespace {
using Clock=std::chrono::steady_clock;
struct Entry { Value value; Clock::time_point fetched{}; bool pending=false; unsigned revision=0; };
std::map<std::string,Entry> cache;
std::string key(int id) { return Utils::serverBaseUrl()+":"+std::to_string(id); }
CCLabelBMFont* findLabel(CCNode* n) {
 if(auto* l=typeinfo_cast<CCLabelBMFont*>(n)) return l;
 for(auto* c:CCArrayExt<CCNode*>(n->getChildren())) if(auto* l=findLabel(c)) return l;
 return nullptr;
}
Value parse(matjson::Value const& body) {
 Value v;
 if(body.isObject()) {
  v.name=body["displayName"].asString().unwrapOr(""); v.color=body["color"].asString().unwrapOr("00FF00");
  v.clickable=body["clickable"].asBool().unwrapOr(false);
 }
 return v;
}
std::string encode(std::string const& s) {
 std::string out; char const* h="0123456789ABCDEF";
 for(unsigned char c:s) {
  if((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='-'||c=='_'||c=='.'||c=='~') out+=c;
  else {out+='%';out+=h[c>>4];out+=h[c&15];}
 }
 return out;
}
}
void State::restore() {
 if(applied && label) { label->setString(original.c_str()); label->setPosition(position); label->setColor(color); }
 if(applied && button) button->setEnabled(enabled);
 applied=false; label=nullptr; button=nullptr;
}
Value cached(int id) { return cache[key(id)].value; }
void apply(CCNode* root,int id,State& state,bool leftAlign) {
 if(id<=0 || !Utils::isOnGdps()) return;
 auto k=key(id); auto& e=cache[k];
 if(!e.pending && (e.fetched==Clock::time_point{} || Clock::now()-e.fetched>std::chrono::seconds(15))) {
  e.pending=true; e.fetched=Clock::now(); auto rev=++e.revision;
  auto url=Utils::serverBaseUrl()+"/creator/level.php?levelID="+std::to_string(id);
  std::thread([k,url,rev]() {
   auto req=web::WebRequest();req.timeout(std::chrono::seconds(6));auto res=req.getSync(url);
   Loader::get()->queueInMainThread([k,rev,res=std::move(res)]() mutable {
    auto& entry=cache[k];if(entry.revision!=rev)return;entry.pending=false;
    auto json=res.json();if(!res.ok()||!json)return;auto b=json.unwrap();
    if(b.isObject()&&b["ok"].asBool().unwrapOr(false))entry.value=parse(b["display"]);
   });
  }).detach();
 }
 auto v=e.value;
 if(v.name.empty()) {state.restore();return;}
 auto* node=root->getChildByIDRecursive("creator-name"); if(!node)return;
 auto* label=findLabel(node);if(!label)return;
 auto* button=typeinfo_cast<CCMenuItem*>(node);
 if(!state.applied || state.label.data()!=label) {
  state.restore();state.label=label;state.button=button;state.original=label->getString();
  state.position=label->getPosition();state.width=label->getContentSize().width;
  state.color=label->getColor();state.enabled=button?button->isEnabled():true;state.applied=true;
 }
 auto name=v.name;
 if(state.original.starts_with("By ")) name="By "+name;
 else if(state.original.starts_with("BY "))name="BY "+name;
 if(button)button->setEnabled(v.clickable);
 label->setString(name.c_str());
 // BMFont changes its size without moving the sprite inside the native button.
 // Preserve the original left edge instead of centering a shorter alias in the old width.
 auto position=state.position;
 if(leftAlign) position.x+=(label->getContentSize().width-state.width)*label->getScaleX()*label->getAnchorPoint().x;
 label->setPosition(position);
 auto hexColor=[](std::string const& hex) {
  auto n=std::stoul(hex,nullptr,16);
  return ccColor3B{static_cast<GLubyte>(n>>16),static_cast<GLubyte>(n>>8),static_cast<GLubyte>(n)};
 };
 if(v.color.size()==6) label->setColor(hexColor(v.color));
 else if(v.color.size()==13 && v.color[6]=='-') {
  label->setColor({255,255,255});
  auto first=hexColor(v.color.substr(0,6)),last=hexColor(v.color.substr(7,6));
  auto* glyphs=label->getChildren();
  auto prefix=name.size()-v.name.size();
  size_t index=0;
  for(auto* child:CCArrayExt<CCNode*>(glyphs)) {
   if(auto* glyph=typeinfo_cast<CCSprite*>(child)) {
    float t=v.name.size()>1 ? static_cast<float>(std::clamp<int>(static_cast<int>(index)-static_cast<int>(prefix),0,static_cast<int>(v.name.size()-1)))/static_cast<float>(v.name.size()-1):0.f;
    glyph->setColor({static_cast<GLubyte>(first.r+(last.r-first.r)*t),static_cast<GLubyte>(first.g+(last.g-first.g)*t),static_cast<GLubyte>(first.b+(last.b-first.b)*t)});
    ++index;
   }
  }
 }
}
void save(int id,Value value,std::function<void(bool,std::string)> done) {
 if(!SessionManager::canManageRoles()){done(false,"Only the server owner can edit creator displays.");return;}
 auto* am=GJAccountManager::sharedState();int account=am->m_accountID;
 std::string credential=am->m_GJP2,server=Utils::serverBaseUrl();auto k=key(id);
 auto body="accountID="+std::to_string(account)+"&gjp2="+encode(credential)+"&levelID="+std::to_string(id)
  +"&displayName="+encode(value.name)+"&color="+encode(value.color)+"&clickable="+(value.clickable?"1":"0");
 std::thread([account,credential,server,k,body,done=std::move(done)]() mutable {
  auto req=web::WebRequest();req.header("Content-Type","application/x-www-form-urlencoded");req.bodyString(body);
  req.timeout(std::chrono::seconds(8));auto res=req.postSync(server+"/creator/set.php");
  Loader::get()->queueInMainThread([account,credential,server,k,res=std::move(res),done=std::move(done)]() mutable {
   auto* am=GJAccountManager::sharedState();
   if(am->m_accountID!=account||std::string(am->m_GJP2)!=credential||Utils::serverBaseUrl()!=server){done(false,"Account or server changed.");return;}
   auto parsed=res.json();if(!parsed){done(false,"Could not save. Check the server connection.");return;}
   auto b=parsed.unwrap();if(!b.isObject()){done(false,"Invalid server response.");return;}if(!res.ok()||!b["ok"].asBool().unwrapOr(false)){done(false,b["error"].asString().unwrapOr("Save denied."));return;}
   auto& e=cache[k];++e.revision;e.pending=false;e.value=parse(b["display"]);e.fetched=Clock::now();
   done(true,"Creator display saved. Other players with the updated mod will see it automatically.");
  });
 }).detach();
}
}
