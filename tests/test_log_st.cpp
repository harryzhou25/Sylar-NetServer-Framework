#include "log/logger.h"

int main() {
    auto logger = sylar::LoggerMgr::getInstance()->getRoot();
    Bug_Log(logger) << "test";
    return 0;
}