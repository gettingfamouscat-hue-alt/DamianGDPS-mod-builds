#pragma once
#include <filesystem>
#include <asp/fs/std.hpp>

namespace gdpsCompat {
inline std::string pathText(std::filesystem::path const& path) {
    auto converted = asp::fs::Path::fromStdPath(path);
    return std::string(converted.string());
}
}
