//
// Created by czlcz on 2022/7/28.
//

#ifndef CPPREQUEST_CONTAINERS_H
#define CPPREQUEST_CONTAINERS_H

#include <string>
#include <algorithm>
#include <chrono>

namespace requests {

/// \note you will have to provide your own iterator
template <typename Container>
class StringContainerBase {
private:
    std::string data_;
public:
//    StringContainerBase() = default;
    explicit StringContainerBase(std::string data = ""): data_ (std::move(data)) {}
    ~StringContainerBase() = default;

    StringContainerBase(const StringContainerBase& other) = default;
    StringContainerBase& operator= (const StringContainerBase& other) = default;

    StringContainerBase(StringContainerBase&& other)  noexcept = default;
    StringContainerBase& operator= (StringContainerBase&& other)  noexcept = default;

    ///< converters
    // to string
    explicit operator std::string() const {
        return data_;
    }
    // to const char*
    explicit operator const char*() const {
        return data_.data();
    }

    ///< string abilities
    const char *data() const noexcept { return data_.data(); }
    const char *c_str() const noexcept { return data_.c_str(); }
    size_t size() const noexcept { return data_.size(); }
    bool empty() const noexcept { return data_.empty(); }
    char& operator[](const size_t index) {return data_[index];}


    Container operator+ (const char* rhs) const {
        return Container{data_ + rhs};
    }
    Container operator+ (const std::string& rhs) const {
        return Container{data_ + rhs};
    }
    Container operator+ (const Container& rhs) const {
        return Container{data_ + rhs.data_};
    }

    /// \fixme is there any way to achieve this?
//    Container& operator+= (const char* rhs) {
//        data_.append(rhs);
//        return *this;
//    }
//    Container& operator+= (const std::string& rhs) {
//        data_.append(rhs);
//        return *this;
//    }
//    Container& operator+= (const Container& rhs) {
//        data_.append(rhs.data_);
//        return *this;
//    }

    void operator+= (const char* rhs) {
        data_.append(rhs);
    }
    void operator+= (const std::string& rhs) {
        data_.append(rhs);
    }
    void operator+= (const Container& rhs) {
        data_.append(rhs.data_);
    }


};

struct caseInsensibleHash: private std::hash<std::string>{
    size_t operator() (const std::string& str) const {
        return std::hash<std::string>::operator()(utils::str_tolower(str));
    }
};

struct caseInsensiblePred {
    bool operator() (const std::string& lhs, const std::string& rhs) const {
        return utils::str_tolower(lhs) == utils::str_tolower(rhs);
    }
};

template <typename Val>
class caseInsensibleUMap {
    using map_type = std::unordered_map<std::string, Val, caseInsensibleHash,  caseInsensiblePred>;
    using iterator = typename map_type::iterator;
    using const_iterator = typename map_type::const_iterator;
    using key_type = std::string;
    using value_type = typename map_type::value_type;
    using pair_type = std::pair<std::string, Val>;
private:
    map_type map_;
public:
    caseInsensibleUMap() = default;
    ~caseInsensibleUMap() = default;
    ///< possible modifiers override

    auto begin() {return map_.begin();}
    auto end() {return map_.end();}
    auto rbegin() {return map_.rbegin();}
    auto rend() {return map_.rbegin();}

    auto at(const key_type& key) {
        return map_.at(to_header_fmt(key));
    }
    std::string& operator[] (const key_type& key) noexcept {
        return map_.operator[](to_header_fmt(key));
    }

    template<class ...Args>
    caseInsensibleUMap<Val>& emplace(const std::string& key, const Val& val) {
        map_.emplace(to_header_fmt(key), val);
        return *this;
    }
    template<class ...Args>
    auto emplace_hint(const_iterator it, const std::string& key, const Val& val) {
        return map_.emplace_hint(it, to_header_fmt(key), val);
    }
    auto insert(pair_type pair) {
        pair.first = to_header_fmt(pair.first);
        return map_.insert(pair);
    }
    auto insert(std::initializer_list<pair_type> ilist) {
        for (auto &pair: ilist) {
            insert(pair);
        }
    }

private:
    std::string to_header_fmt(const std::string& key) {
        std::string res{};
        if (key.empty())
            return res;
        auto vec = utils::split(utils::str_tolower(key), '-');
        for (auto i=0; i<vec.size()-1; ++i) {
            if (vec[i].empty())
                continue;
            vec[i].front() = std::toupper(vec[i].front());
            res.append(vec[i]).append("-");
        }
        vec.back().front() = std::toupper(vec.back().front());
        res.append(vec.back());
        return res;
    }
};

struct Cookie {
    std::string path;
    std::string domain;
    std::string max_age;
    std::string version;
    std::string comment;
    std::string same_site;
    bool http_only;
    std::chrono::system_clock::time_point expires;
    std::pair<std::string, std::string> pair; ///< name-value pair

    Cookie(std::string raw_cookie);
    Cookie(const std::string& name,
           const std::string& value,
           const std::string& path="",
           const std::string& domain="",
           const std::string& max_age="",
           const std::string& version="",
           const std::string& comment="",
           const std::string& http_only="",
           const std::string& same_site="",
           const std::string& expires="");

    std::string set(); ///< Set-Cookie
    std::string get(); ///< Cookie
};

class Header {
private:
    std::string data_;
//    caseInsensibleUMap<std::string> map_;

//    bool encoded_;
public:
    std::string& operator[] (const std::string& key) noexcept;
    std::string& at(const std::string& key);

    const std::string& to_string();
//    std::s
private:
    using pair_t = std::pair<std::string, std::string>;
    int try_add(const std::string& line);
    int try_add(const pair_t& pair);
};

//class caseInsensibleUMap{
//private:
//    std::unordered_map<std::string, std::string, std::hash<std::string>, caseInsensiblePred> map_;
//public:
//    std::string to_string();
//    std::string to_string(const std::string& between_pair,
//                        const std::string& between_line);
//};
//
//    std::string caseInsensibleUMap::to_string(const std::string &between_pair, const std::string &between_line) {
//        std::string text;
//        for (auto &pair: map_) {
//            text.append(pair.first)
//                .append(between_pair)
//                .append(pair.second)
//                .append(between_line);
//        }
//        return text;
//    }
//
//    std::string caseInsensibleUMap::to_string() {
//        return to_string(": ", "\r\n");
//    }
} // namespace requests
#endif //CPPREQUEST_CONTAINERS_H
