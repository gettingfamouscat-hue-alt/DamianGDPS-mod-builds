#pragma once
#include <string_view>
namespace updater {
constexpr bool autoUpdateAllowed(std::string_view id) {
    return id != "sabe.platformersaves";
}
}
