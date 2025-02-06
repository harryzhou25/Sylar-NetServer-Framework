#include "log/logger.h"
#include "thread/thread.h"

#include <vector>
#include <string>
#include <iostream>

void run() {
    auto logger = sylar::LoggerMgr::getInstance()->getRoot();
    Log_Bug(logger) << "test";
}

int main() {
    for(int i = 0; i < 10; ++i) {
        auto tmp = sylar::Thread(std::forward<void()>(run), "test_" + std::to_string(i));
    }
    return 0;
}