#pragma once
#include <Geode/Geode.hpp>
#include <map>
#include <functional>
namespace StaffRatingClient {
using Fields = std::map<std::string, std::string>;
using Callback = std::function<void(bool, std::string, matjson::Value)>;
void request(int level, std::string action, Fields fields, Callback done);
void refreshLevel(int level);
}
