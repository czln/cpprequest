//
// Created by czlcz on 2022/7/28.
//

#ifndef CPPREQUEST_STRING_CONTAINER_BASE_H
#define CPPREQUEST_STRING_CONTAINER_BASE_H

#include <string>
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
} // namespace requests
#endif //CPPREQUEST_STRING_CONTAINER_BASE_H
