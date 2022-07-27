//
// Created by czlcz on 2022/7/27.
//

#include "utils.h"
namespace utils {
std::vector<std::string> split(const std::string& str, const char by) {
    std::vector<std::string> res;
    size_t last = 0;
    for (auto i=0; i<str.size(); ++i) {
        if (str[i] == by) {
            res.emplace_back(str.data(), last, i-last+1);
            last = i;
        }
    }
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
} // namespace utils