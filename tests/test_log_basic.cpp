#include "log/logger.h"

int main() {
    sylar::Logger::Ptr logger(new sylar::Logger);
    sylar::LogFormatter::Ptr fmt(new sylar::LogFormatter("%d %T %p %T %m %n"));
    logger->addAppender(sylar::LogAppender::Ptr(new sylar::StdoutLogAppender));
    logger->addAppender(sylar::LogAppender::Ptr(new sylar::FileLogAppender("log/log_test")));

    sylar::StdoutLogAppender::Ptr appender(new sylar::StdoutLogAppender);
    appender->setFormatter(fmt);
    logger->addAppender(appender);
    return 0;
}