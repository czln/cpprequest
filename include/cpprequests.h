//
// Created by czlcz on 2022/7/27.
//

#ifndef CPPREQUEST_CPPREQUEST_H
#define CPPREQUEST_CPPREQUEST_H

#include <curl/curl.h>
#include <string>
#include <unordered_map>
//#include <utility>
#include <functional>
#include <memory>
#include <vector>
#include <future>

#include "utils.h"
#include "containers.h"



namespace requests {
/// \note if there're multiple  key-val with the same key, the val will be joined together,
/// since the header already separated by \c "\\r\\n"
//    struct  Header {
//    private:
//        std::unordered_map<std::string, std::string> map_;
//        std::string text_; ///< original plain text
//    public:
//        explicit Header(const std::string& str = "") :text_(str) {
//            add(str);
//        }
//        std::string operator[](const std::string& key) {
//            ///< so that map_ will match the text_
//            if (map_.find(key) == map_.end())
//                return {};
//            return map_[key];
//        }
////    Header& operator+ (const std::string& data) {
////        text_
////        return *this;
////    }
//        Header& operator+= (const std::string& data) {
//            text_ += data;
//            add(data);
//            return *this;
//        }
//        auto begin() {return map_.begin();}
//        auto end() {return map_.end();}
//        const char* data() noexcept{ return text_.data(); }
//        size_t size() const noexcept { return text_.size(); }
//    private:
//        void add(const std::string& data) {
//            if (!data.empty() && data.find(':') != std::string::npos) {
//#if __cplusplus > 201700L
//                auto [ key,  val] = utils::separate(data, ':');
//#else
//                auto pair = utils::separate(data, ':');
//                auto key = pair.first, val = pair.second;
//#endif
//                if (map_.find(key) == map_.end()) {
//                    map_.emplace(key, val);
//                } else {
//                    map_[key] += val; ///< separated by \c "\r\n"
//                }
//            }
//        }
//    };
//    struct Header: public StringContainerBase<Header>{
//    private:
//        std::unordered_map<std::string, std::string> map_;
//    public:
//        Header() = default;
//        Header(const char* data) : StringContainerBase<Header>(data) {}
//        Header(const char* data, size_t n) : StringContainerBase<Header>(std::string{data, n}) {}
//        Header(const std::string& data) : StringContainerBase<Header>(data) {}
//
//        Header(const Header& other) = default;
//        Header& operator= (const Header& other) = default;
//
//        Header(Header&& other) noexcept = default;
//        Header& operator= (Header&& other) noexcept = default;
//
//        auto begin() {return map_.begin();}
//        auto end() {return map_.end();}
//    };
    struct Response {
        struct  Text {};
        uint    status_code{};
        Header  header;
        Text    text;
    };

    struct Url: public StringContainerBase<Url>{
        Url() = default;
        Url(const char* data) : StringContainerBase<Url>(data) {}
        Url(const char* data, size_t n) : StringContainerBase<Url>(std::string{data, n}) {}
        Url(const std::string& data) : StringContainerBase<Url>(data) {}

        Url(const Url& other) = default;
        Url& operator= (const Url& other) = default;

        Url(Url&& other) noexcept = default;
        Url& operator= (Url&& other) noexcept = default;
    };

    class Verbose {
    private:
        bool enable_;
    public:
        Verbose(const bool enable = false) : enable_(enable) {}
        explicit operator bool() const {
            return enable_;
        }
    };

    using header_callback_t = std::function<size_t(const std::string& data)>;
    using on_data_dispatch_t = std::function<size_t(const std::string& data)>;

    struct OnDataCallbackBase {
        on_data_dispatch_t cb_;
        explicit OnDataCallbackBase(on_data_dispatch_t cb = {}) :cb_(std::move(cb)) {}
        size_t operator()(const std::string& header) const {
            if (!cb_) {
                fprintf(stderr, "warning: no callback\n");
                return 0;
            }
            return cb_(header);
        }
    };

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
    static size_t on_data_dispatch(const char* data, size_t chunk_size, size_t n_chunk, OnDataCallbackBase* cb){
        if (!cb)
            return 0;
//    std::string s(data, chunk_size * n_chunk);
        return cb->operator()({data, chunk_size * n_chunk});
    }

//static size_t header_func(const char* data, size_t chunk_size, size_t n_chunk, HeaderCallback* cb) {
//    if (!cb)
//        return 0;
////    std::string s(data, chunk_size * n_chunk);
//    return (*cb)({data, chunk_size * n_chunk});
//}
//static size_t write_func(const char* data, size_t chunk_size, size_t n_chunk, void* cb) {
//    if (!cb)
//        return 0;
////    std::string s(data, chunk_size * n_chunk);
//    return (*cb)({data, chunk_size * n_chunk});
//    return 0;
//}

    struct FutureResponse;

    class Session: public std::enable_shared_from_this<Session> {
    private:
        Url url_;
        Verbose verbose_;
        Header header_;
        CURL* handle_;

        /// callbacks
        std::unique_ptr<OnDataCallbackBase> header_callback_;
        std::unique_ptr<OnDataCallbackBase> write_callback_;
        Session() {
            handle_ = curl_easy_init();
        }

    public:
        ~Session() {
            curl_easy_cleanup(handle_);
        }
        static std::shared_ptr<Session> create() {
            return std::shared_ptr<Session>(new Session);
        }
        void set_option(Url url) { url_ = std::move(url); }
        void set_option(Verbose verbose) { verbose_ = verbose; }

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
            if (!header_callback_) {
                header_callback_ = std::make_unique<OnDataCallbackBase>([this](const std::string& data) {
//                    header_ += data;
                    return data.size();
                });
            }
            curl_easy_setopt(handle_, CURLOPT_HEADERFUNCTION, on_data_dispatch);
            curl_easy_setopt(handle_, CURLOPT_HEADERDATA, header_callback_.get());
            curl_easy_setopt(handle_, CURLOPT_WRITEFUNCTION, on_data_dispatch);
            curl_easy_setopt(handle_, CURLOPT_WRITEDATA, nullptr);
        }
        void prepare() {
            curl_easy_setopt(handle_, CURLOPT_URL, url_.data());
            if (verbose_)
                curl_easy_setopt(handle_, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(handle_, CURLOPT_HEADER, false);
            setup_callback();
        }
    };

    template <typename Arg>
    Response get_imp(const std::shared_ptr<Session>& session, Arg arg) {
        session->set_option(std::forward<Arg&&>(arg));
        return session->get();
    }
    template <typename Arg1, typename ...Args>
    Response get_imp(std::shared_ptr<Session> session, Arg1 arg1, Args... args) {
        session->set_option(std::forward<Arg1&&>(arg1));
        return get_imp(session, std::forward<Args...>(args...));
    }

    template <typename ...Args>
    Response get(Args... args) {
        auto session = Session::create();
        return get_imp(session, args...);
    }

    struct FutureResponse {
        std::future<Response> response;
        std::weak_ptr<Session> session; ///< so that you can call cancel with session
    };


    template <typename Arg>
    FutureResponse get_async_imp(const std::shared_ptr<Session>& session, Arg arg) {
        session->set_option(std::forward<Arg&&>(arg));
        return {
                std::async([session]() {return session->get();}),
                session
        };
    }
    template <typename Arg1, typename ...Args>
    FutureResponse get_async_imp(std::shared_ptr<Session> session, Arg1 arg1, Args... args) {
        session->set_option(std::forward<Arg1&&>(arg1));
        return get_async_imp(session, std::forward<Args...>(args...));
    }

    template <typename ...Args>
    FutureResponse get_async(Args... args) {
        auto session = Session::create();
        return get_async_imp(session, args...);
    }


    enum E_REQ_STATUS {
        E_REQ_SUCCESS,
        E_REQ_ERROR,
        E_REQ_CANCEL,
    };

    E_REQ_STATUS get_status(uint code) {
        if (code >= 200 && code < 300)
            return E_REQ_SUCCESS;
        /// \todo other status return
        return E_REQ_ERROR;
    }

} // namespace requests


#endif //CPPREQUEST_CPPREQUEST_H
