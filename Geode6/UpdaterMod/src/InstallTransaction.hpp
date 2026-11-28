#pragma once
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

namespace updater::install {
namespace fs = std::filesystem;
inline bool safeName(std::string const& name) {
    return !name.empty() && name != "." && name != ".." && name.find_first_of("/\\:\r\n\t") == std::string::npos;
}
inline bool safeID(std::string const& id) {
    if (id.empty() || id.size()>64 || id.front()=='.' || id.back()=='.' || std::count(id.begin(),id.end(),'.')!=1) return false;
    return std::all_of(id.begin(),id.end(),[](unsigned char c) { return (c>='a' && c<='z') || (c>='0' && c<='9') || c=='.' || c=='-' || c=='_'; });
}
inline void replace(fs::path const& source, fs::path const& target) {
#ifdef _WIN32
    if (!MoveFileExW(source.c_str(), target.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        throw std::runtime_error("Could not replace " + target.filename().string() + " (Windows error " + std::to_string(GetLastError()) + ").");
#else
    fs::rename(source, target);
#endif
}
struct Package { std::string id; fs::path staged; std::vector<fs::path> previous; };
struct Original { fs::path target, backup; bool existed = false; };
/** All sources must already have passed hash, metadata and dependency checks. */
inline void commit(fs::path const& mods, fs::path const& backup, std::vector<Package> const& packages,
                   std::function<void(size_t)> afterWrite = {}) {
    auto root = fs::canonical(mods);
    if (packages.empty() || fs::exists(backup)) throw std::runtime_error("Invalid update transaction.");
    fs::create_directories(backup);
    std::vector<Original> originals;
    std::set<fs::path> seen;
    std::set<std::string> ids;
    // Finish all backups before changing any installed package.
    for (auto const& package : packages) {
        if (!safeID(package.id) || !ids.insert(package.id).second || !fs::is_regular_file(package.staged) || fs::is_symlink(package.staged))
            throw std::runtime_error("Invalid staged package.");
        auto paths = package.previous; paths.push_back(root/(package.id+".geode"));
        for (auto const& p : paths) {
            if (!safeName(p.filename().string()) || fs::canonical(p.parent_path()) != root || fs::is_symlink(p))
                throw std::runtime_error("A mod package path is outside the Mods folder.");
            auto target = root/p.filename();
            if (!seen.insert(target).second) continue;
            Original old{target,backup/(std::to_string(originals.size())+".geode"),fs::exists(target)};
            if (old.existed) fs::copy_file(target,old.backup);
            originals.push_back(std::move(old));
        }
    }
    std::ofstream journal(backup/"files.txt");
    for (auto const& old : originals) journal << old.target.filename().string() << "\t" << old.backup.filename().string() << "\t" << old.existed << '\n';
    journal.close();
    if (!journal) throw std::runtime_error("Could not save the update backup record.");
    try {
        size_t count = 0;
        for (auto const& package : packages) {
            replace(package.staged,root/(package.id+".geode"));
            if (afterWrite) afterWrite(++count);
        }
        for (auto const& package : packages) for (auto const& old : package.previous)
            if (old.filename() != fs::path(package.id+".geode")) fs::remove(old);
        std::ofstream done(backup/"complete.txt"); done << "Update installed. Original packages are retained here.\n";
        done.close(); if (!done) throw std::runtime_error("Could not finish the update record.");
    } catch (...) {
        auto error = std::current_exception(); bool restored = true;
        for (auto const& old : originals) {
            try {
                if (old.existed) {
                    auto temporary = root/(old.target.filename().string()+".restore-update");
                    fs::copy_file(old.backup,temporary,fs::copy_options::overwrite_existing);
                    replace(temporary,old.target);
                } else fs::remove(old.target);
            } catch (...) { restored = false; }
        }
        if (!restored) throw std::runtime_error("The update failed and some packages could not be restored. Original packages are in the update backup folder.");
        std::rethrow_exception(error);
    }
}
}
