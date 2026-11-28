#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace GoldenCoin {
// Match the native, nonrewarding coin in official level 130684749.
// These flags are serialized only; modded play enables cosmetic pickup.
inline std::string officialCoinData(std::string_view data) {
    std::string output;
    size_t start = 0;
    while (start < data.size()) {
        auto end = data.find(';', start);
        if (end == std::string_view::npos) end = data.size();
        auto object = data.substr(start, end - start);
        std::vector<std::string_view> fields;
        size_t fieldStart = 0;
        while (fieldStart < object.size()) {
            auto fieldEnd = object.find(',', fieldStart);
            if (fieldEnd == std::string_view::npos) fieldEnd = object.size();
            fields.push_back(object.substr(fieldStart, fieldEnd - fieldStart));
            fieldStart = fieldEnd + 1;
        }
        bool coin = false;
        if (fields.size() % 2 == 0)
            for (size_t i = 0; i < fields.size(); i += 2)
                if (fields[i] == "1" && fields[i + 1] == "142") coin = true;
        if (coin) {
            bool animationSpeed = false;
            bool first = true;
            for (size_t i = 0; i < fields.size(); i += 2) {
                auto key = fields[i];
                // 381 is pickup-item behavior set by our runtime collection
                // path, not part of the stock-compatible secret coin data.
                if (key == "12" || key == "121" || key == "36" || key == "381") continue;
                if (!first) output += ',';
                output.append(key);
                output += ',';
                output.append(fields[i + 1]);
                first = false;
                if (key == "107") animationSpeed = true;
            }
            output += ",121,1,36,1";
            if (!animationSpeed) output += ",107,1";
        } else output.append(object);
        if (end < data.size()) output += ';';
        start = end + 1;
    }
    return output;
}
}
