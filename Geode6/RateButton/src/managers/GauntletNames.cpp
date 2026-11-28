#include "GauntletNames.hpp"
#include "../utils/Utils.hpp"
#include <Geode/modify/GauntletNode.hpp>
#include <charconv>
#include <sstream>
using namespace geode::prelude;
namespace GauntletNames {
void read(std::string const& response) {
    if (!Utils::isOnGdps() || response.empty() || response[0] == '-' || response.find('#') == std::string::npos) return;
    auto names = matjson::Value::object();
    std::istringstream rows(response.substr(0, response.find('#'))); std::string row;
    while (std::getline(rows, row, '|')) {
        std::istringstream fields(row); std::string key, value, name; int id = 0;
        while (std::getline(fields, key, ':') && std::getline(fields, value, ':')) {
            if (key == "1") {
                auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), id);
                if (error != std::errc() || end != value.data() + value.size()) id = 0;
            }
            if (key == "2" && value.size() <= 80) name = value;
        }
        if (id >= 1 && id <= 62 && !name.empty()) names[std::to_string(id)] = name;
    }
    Mod::get()->setSavedValue("gauntlet-names-" + Utils::serverBaseUrl(), names);
}
std::string get(GauntletType type) {
    if (!Utils::isOnGdps()) return {};
    auto names = Mod::get()->getSavedValue<matjson::Value>("gauntlet-names-" + Utils::serverBaseUrl());
    return names[std::to_string(static_cast<int>(type))].asString().unwrapOr("");
}
}
class $modify(GauntletNode) {
    static std::string nameForType(GauntletType type) {
        auto name = GauntletNames::get(type);
        return name.empty() ? GauntletNode::nameForType(type) : name;
    }
};
