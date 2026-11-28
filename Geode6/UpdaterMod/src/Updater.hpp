#pragma once
#include <Geode/Geode.hpp>
#include <atomic>
#include <map>
#include <mutex>

namespace updater {
struct State {
    std::mutex mutex;
    std::string message = "Checking for the latest GDPS pack...", error;
    std::atomic<int> phase{0}; // 0: downloading, 1: cancelled, 2: installing
    std::atomic<bool> done{false};
    std::atomic<int> percentage{-1};
    std::vector<std::string> enable;
    void status(std::string text) { std::lock_guard lock(mutex); message = std::move(text); percentage = -1; }
};
bool busy();
std::shared_ptr<State> start();
}
