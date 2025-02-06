#include "log/logger.h"

int main() {
    auto logger = sylar::LoggerMgr::getInstance()->getRoot();
    Log_Bug(logger) << "test";
    return 0;
}