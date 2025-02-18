#include "socket/address.h"
#include "log/logger.h"
#include <iostream>

int main() {
    std::vector<sylar::Address::Ptr> result;
    bool res = sylar::Address::Lookup(result, "www.google.com", AF_INET);
    if(res) {
        for(auto &i : result) {
            std::cout << i->toString() << '\n';
        }
    }
    else {
        std::cout << "lookup failed" << '\n';
    }
    return 0;
}