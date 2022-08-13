//
// Created by czlcz on 2022/7/27.
//

#include "utils.h"

#include <algorithm>

namespace utils {
std::vector<std::string> split(const std::string& str, const char by) {
    std::vector<std::string> res;
    size_t last = 0, i;
    for (i=0; i<str.size(); ++i) {
        if (str[i] == by) {
            res.emplace_back(str.data(), last, i-last);
            last = i + 1;
        }
    }
    res.emplace_back(str.data(), last, i-last);
    return res;
}

std::pair<std::string, std::string> separate(const std::string& str, const char by) {
    std::pair<std::string, std::string> res;
    for (auto i=0; i<str.size(); ++i) {
        if (str[i] == by) {
            res.first = str.substr(0, i);
            res.second = str.substr(i+1, str.size()-i);
            break;
        }
    }
    return res;
}


std::string str_tolower(const std::string& raw) {
    std::string res{raw};
    std::transform(raw.begin(), raw.end(), res.begin(),
                   [](const unsigned char ch) {
                       return std::tolower(ch);
    });
    return res;
}

} // namespace utils