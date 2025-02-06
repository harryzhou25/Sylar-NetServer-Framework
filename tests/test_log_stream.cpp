#include "log/logger.h"

int main() {
    sylar::Logger::Ptr logger(new sylar::Logger);
    sylar::LogFormatter::Ptr fmt(new sylar::LogFormatter("%d %T %p %T %m %n"));
    logger->addAppender(sylar::LogAppender::Ptr(new sylar::StdoutLogAppender));
    logger->addAppender(sylar::LogAppender::Ptr(new sylar::FileLogAppender("log/log_test")));

    sylar::LogEvent::Ptr event(new sylar::LogEvent(logger, sylar::LogLevel::INFO, __FILE__, int32_t(0), uint32_t(0), uint32_t(0), uint32_t(0), uint64_t(0), ""));
    logger->log(sylar::LogLevel::INFO, event);

    Bug_Log(logger) << "test";
    return 0;
}