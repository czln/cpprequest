//
// Created by czlcz on 2022/7/27.
//

#include "cpprequests/requests.h"

int main() {
    using namespace requests;
    Response response = requests::get(Url{"www.baidu.com"}, Verbose{true});
    printf("\n**************************\n");
    printf("header:\n%s\n", response.header.data());
    printf("\n**************************\n");
}