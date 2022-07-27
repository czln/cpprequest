//
// Created by czlcz on 2022/7/27.
//

#ifndef CPPREQUEST_UTILS_H
#define CPPREQUEST_UTILS_H

#include <string>
#include <vector>
namespace utils {
std::vector<std::string> split(const std::string& str, const char by);

std::pair<std::string, std::string> separate(const std::string& str, const char by);

}; // namespace utils
#endif //CPPREQUEST_UTILS_H
