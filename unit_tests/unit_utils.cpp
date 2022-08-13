//
// Created by zelin on 2022/8/10.
//

#include "cpprequests/requests.h"

#include <cassert>
#include <initializer_list>

using namespace requests;

int main() {
    auto raw = " Qwe12 3ewQ ";
    auto res = " qwe12 3ewq ";
    auto s = utils::str_tolower(raw);
    assert(s == res);

    auto v = utils::split("123", '1');
    std::vector<std::string> r = {"", "23"};
    assert(v == r);
    v = utils::split("123", '2');
    r = {"1", "3"};
    assert(v == r);
    v = utils::split("123", '3');
    r = {"12", ""};
    assert(v == r);

    caseInsensibleUMap<std::string> map;
    map["aA"] = "123";
    map["AA"] = "122";
    map["Aa"] = "234";
    assert(map.at("aa") == "234");
    map["qwe-qwe"];
    assert(map.at("Qwe-Qwe").empty());
    map.emplace("1", "");
    assert(map.at("1").empty());
    map.emplace("ew", "123");
    assert(map.at("ew")=="123");
    map.insert({"ew", "23"});
    assert(map.at("ew")=="123");

    for (auto &pair: map) {
        printf("%s: %s\n", pair.first.data(), pair.second.data());
    }
    return 0;
}