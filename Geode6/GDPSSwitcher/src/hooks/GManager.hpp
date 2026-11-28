#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GManager.hpp>

using namespace geode::prelude;

class $modify(GSGManager, GManager) {

    struct Fields {
        static std::vector<GSGManager*> m_managers;
        std::string m_originalFileName;
    };

    void setup() override;
    bool save(bool async);
    static void updateFileNames();
};