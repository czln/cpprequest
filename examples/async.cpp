//
// Created by czlcz on 2022/7/27.
//

#include "cpprequests/requests.h"
#include <thread>

int main() {
    using namespace requests;
    auto future_response = requests::get_async(Url{"www.baidu.com"});
    std::thread p([&future_response]() {
        auto response = future_response.response.get();
        switch (requests::get_status(response.status_code)) {
        case requests::E_REQ_SUCCESS:
            break;
        default:
            fprintf(stderr, "fail to get\n");
        }
        printf("header in thread\n%s\n", response.header.data());
    });
    p.join();
    return 0;
}