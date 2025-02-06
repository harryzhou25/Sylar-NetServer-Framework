#include "log/logger.h"
#include "thread/Mutex.h"
#include "thread/thread.h"

#include <vector>
#include <string>
#include <iostream>

static int cnt = 0;
std::shared_mutex mtx;

void run() {
    for(int i = 0; i < 100000; ++i) {
        writeLock(mtx);
        ++cnt;
    }
    {
        readLock(mtx);
        auto logger = sylar::LoggerMgr::getInstance()->getRoot();
        Log_Bug(logger) << cnt;
    }
}

int main() {
    std::vector<sylar::Thread::Ptr> ths;
    for(int i = 0; i < 10; ++i) {
        sylar::Thread::Ptr tmp(new sylar::Thread(std::forward<void()>(run), "test_" + std::to_string(i)));
        ths.push_back(tmp);
    }
    for(auto &i : ths) {
        i->join();
    }
    return 0;
}