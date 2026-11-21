#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include "ListManager.h"

#include <thread>
#include <utility>

using namespace geode::prelude;

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        if (ListManager::firstTimeOpen) return true;
        ListManager::firstTimeOpen = true;

        std::thread([] {
            auto request = web::WebRequest();
            request.userAgent("GrandpaDemonMod/2.0");
            auto response = request.getSync("https://api.aredl.net/api/aredl/levels");
            if (!response.ok()) {
                log::warn("Grandpa Demon: AREDL returned HTTP {}", response.code());
                return;
            }
            auto json = response.json();
            if (json.isErr()) {
                log::warn("Grandpa Demon: could not parse AREDL data");
                return;
            }
            auto data = json.unwrap();
            Loader::get()->queueInMainThread([data = std::move(data)] {
                ListManager::parseData(data);
                ListManager::filterType = -1;
                log::info("Grandpa Demon: loaded {} AREDL levels", ListManager::demonIDList.size());
            });
        }).detach();
        return true;
    }
};
