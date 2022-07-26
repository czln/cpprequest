#include <uuid/uuid.h>
#include <curl/curl.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <functional>
#include <memory>
#include <vector>

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


namespace requests {
/// \note if there're multiple  key-val with the same key, the val will be joined together,
/// since the header already separated by \c "\\r\\n"
struct  Header {
private:
    std::unordered_map<std::string, std::string> map_;
    std::string text_; ///< original plain text
public:
//    class iterator {
//        using map_t = std::unordered_map<std::string, std::string>;
//    private:
//        map_t &map_;
//    public:
//        iterator(map_t& map) : map_(map) {}
//        bool operator== (const iterator& other) {
//            return map_ == other.map_;
//        }
//        iterator& operator++() {
//
//        }
//    };
    explicit Header(const std::string& str = "") :text_(str) {
        add(str);
    }
    std::string operator[](const std::string& key) {
        ///< so that map_ will match the text_
        if (map_.find(key) == map_.end())
            return {};
        return map_[key];
    }
//    Header& operator+ (const std::string& data) {
//        text_
//        return *this;
//    }
    Header& operator+= (const std::string& data) {
        text_ += data;
        add(data);
        return *this;
    }
    auto begin() {return map_.begin();}
    auto end() {return map_.end();}
    const char* data() noexcept{ return text_.data(); }
    size_t size() const noexcept { return text_.size(); }
private:
    void add(const std::string& data) {
        if (!data.empty() && data.find(':') != std::string::npos) {
            auto [ key,  val] = separate(data, ':');
            if (map_.find(key) == map_.end()) {
                map_.emplace(key, val);
            } else {
                map_[key] += val; ///< separated by \c "\r\n"
            }
        }
    }
};
struct Response {
    struct  Text {};
    uint    status_code{};
    Header  header;
    Text    text;
};

class Url {
private:
    std::string data_;
public:
    explicit Url(std::string url = ""): data_(std::move(url)) {}
    explicit operator std::string() const {
        return data_;
    }
    const char* data() noexcept {return data_.data();}
};

class Verbose {
private:
    bool enable_;
public:
    explicit Verbose(const bool enable = false) : enable_(enable) {}
    explicit operator bool() const {
        return enable_;
    }
};

using header_callback_t = std::function<size_t(const std::string& data)>;

struct HeaderCallback {
    header_callback_t cb_;
    explicit HeaderCallback(header_callback_t cb = {}) :cb_(std::move(cb)) {}
    size_t operator()(const std::string& header) const {
        if (!cb_) {
            fprintf(stderr, "warning: no callback\n");
            return 0;
        }
        return cb_(header);
    }
};

static size_t header_func(const char* data, size_t chunk_size, size_t n_chunk, HeaderCallback* cb) {
//    std::string s(data, chunk_size * n_chunk);
    return (*cb)({data, chunk_size * n_chunk});
}
static size_t write_func(const char* data, size_t chunk_size, size_t n_chunk, void* cb) {
//    std::string s(data, chunk_size * n_chunk);
//    return (*cb)({data, chunk_size * n_chunk});
    return 0;
}

class Session: public std::enable_shared_from_this<Session> {
private:
    Url url_;
    Verbose verbose_;
    Header header_;
    CURL* handle_;

    /// callbacks
    HeaderCallback hcb_;
public:
    Session() {
        handle_ = curl_easy_init();
    }
    ~Session() {
        curl_easy_cleanup(handle_);
    }
    void set_url(Url url) { url_ = std::move(url); }
    void set_verbose(Verbose verbose) { verbose_ = verbose; }

    Response get() {
        prepare();
        curl_easy_setopt(handle_, CURLOPT_CUSTOMREQUEST, "GET");
        auto ret = curl_easy_perform(handle_);
        Response response;
        response.header = header_;
        response.status_code = ret;
        return response;

    }
private:
    void setup_callback() {
        hcb_ = HeaderCallback{[this](const std::string& data) {
            header_ += data;
            return data.size();
        }};
        curl_easy_setopt(handle_, CURLOPT_HEADERFUNCTION, header_func);
        curl_easy_setopt(handle_, CURLOPT_HEADERDATA, &hcb_);
        curl_easy_setopt(handle_, CURLOPT_WRITEFUNCTION, write_func);
        curl_easy_setopt(handle_, CURLOPT_WRITEDATA, nullptr);
    }
    void prepare() {
        curl_easy_setopt(handle_, CURLOPT_URL, url_.data());
//        curl_easy_setopt(handle_, CURLOPT_VERBOSE, !verbose_);
        curl_easy_setopt(handle_, CURLOPT_HEADER, false);
        setup_callback();
    }
};

template <typename ...Args>
Response get_imp(Url url, Args... args) {
    Session s;
    s.set_url(std::move(url));
    return s.get();
}
template <typename ...Args>
Response get_imp(Verbose enable, Args... args) {
    Session s;
    s.set_verbose(enable);
    return s.get();
}

template <typename ...Args>
Response get(Args... args) {
    return get_imp(args...);
}
} // namespace requests

int main() {
    using namespace requests;
    Response response = requests::get(Url{"www.baidu.com"});
    for (auto & pair : response.header) {
        printf("[%s] : [%s]\n", pair.first.data(), pair.second.data());
    }
    printf("\n**************************\n");
    printf("header:\n%s\n", response.header.data());
    printf("\n**************************\n");
    return 0;
}
