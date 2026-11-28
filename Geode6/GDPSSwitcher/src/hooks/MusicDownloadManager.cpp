#include "utils/PathCompat.hpp"
#include <filesystem>
#include <asp/fs/std.hpp>
#include <Geode/Geode.hpp>
#include <Geode/modify/MusicDownloadManager.hpp>
#include <utility>
#include <utils/GDPSMain.hpp>

using namespace geode::prelude;

class $modify(GDPSMDL, MusicDownloadManager) {

    std::string pathForSFXFolder(int id) {
        if (!GDPSMain::get()->isActive()) return MusicDownloadManager::pathForSFXFolder(id);
        auto server = GDPSMain::get()->m_servers[GDPSMain::get()->m_currentServer];
        return gdpsCompat::pathText(dirs::getSaveDir().toStdPath().unwrap() / "gdpses" / server->saveDir / "");
    }

    std::string pathForSongFolder(int id) {
        if (!GDPSMain::get()->isActive()) return MusicDownloadManager::pathForSongFolder(id);
        auto server = GDPSMain::get()->m_servers[GDPSMain::get()->m_currentServer];
        return gdpsCompat::pathText(dirs::getSaveDir().toStdPath().unwrap() / "gdpses" / server->saveDir / "");
    }
};
