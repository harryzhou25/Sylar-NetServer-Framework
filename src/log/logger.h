#ifndef _SYLAR_LOGGER_H
#define _SYLAR_LOGGER_H

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdarg.h>
#include <map>
#include <mutex>
#include <functional>
#include <sys/syscall.h>
#include <unistd.h>

#include "util/util.h"
#include "thread/thread.h"
#include "util/Singleton.h"

#define STREAM_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        sylar::LogEventWrap(sylar::LogEvent::Ptr(new sylar::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, sylar::getThreadId(),\
                sylar::getFiberId(), time(0), sylar::Thread::t_getName()))).getSS()

#define Log_Debug(logger) STREAM_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)

#define Log_Info(logger) STREAM_LOG_LEVEL(logger, sylar::LogLevel::INFO)

#define Log_Warn(logger) STREAM_LOG_LEVEL(logger, sylar::LogLevel::WARN)

#define Log_Error(logger) STREAM_LOG_LEVEL(logger, sylar::LogLevel::ERROR)

#define Log_Fatal(logger) STREAM_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

#define Root_Logger() sylar::LoggerMgr::getInstance()->getRoot()

#define Name_Logger(name) sylar::LoggerMgr::getInstance()->addLogger(name)

namespace sylar {

    //%d{%Y-%m-%d %H:%M:%S} 
static const std::string DEFAULT_FORMAT = "%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n";

class Logger;

class LogLevel {
public:
    enum Level {
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };
    static const char* ToString(LogLevel::Level level);
    
    static LogLevel::Level FromString(const std::string& str);
};

class LogEvent {
public:
    using Ptr = std::shared_ptr<LogEvent>;
    
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
            ,const char* file, int32_t line, uint32_t elapse
            ,uint32_t thread_id, uint32_t fiber_id, uint64_t time
            ,const std::string& thread_name);

    const char* getFile() const { return m_file;}

    int32_t getLine() const { return m_line;}

    uint32_t getElapse() const { return m_elapse;}

    uint32_t getThreadId() const { return m_threadId;}

    uint32_t getFiberId() const { return m_fiberId;}

    uint64_t getTime() const { return m_time;}

    const std::string& getThreadName() const { return m_threadName;}

    std::string getContent() const { return m_ss.str();}

    std::shared_ptr<Logger> getLogger() const { return m_logger;}

    LogLevel::Level getLevel() const { return m_level;}

    std::stringstream& getSS() { return m_ss;}

    void format(const char* fmt, ...);

    void format(const char* fmt, va_list al);

private:
    const char* m_file = nullptr;       // 文件名
    int32_t m_line = 0;                 // 行号
    uint32_t m_elapse = 0;              // 程序启动到现在的毫秒数
    uint32_t m_threadId = 0;            // 线程号
    uint32_t m_fiberId = 0;             // 协程号
    uint64_t m_time = 0;                // 事件戳
    std::string m_threadName;           // 线程名
    std::stringstream m_ss;             // 日志内容流
    std::shared_ptr<Logger> m_logger;   // 日志器
    // Logger::Ptr m_logger;
    LogLevel::Level m_level;            // 日志等级
};

class LogEventWrap {
public:
    using Ptr = std::shared_ptr<LogEventWrap>;

    LogEventWrap(LogEvent::Ptr e);


    ~LogEventWrap();

    LogEvent::Ptr getEvent() const { return m_event;}
    
    std::stringstream& getSS();
private:
    /**
     * @brief 日志事件
     */
    LogEvent::Ptr m_event;
};

/**
 * @brief 日志格式器
 */
class LogFormatter {
public:
    /** 格式说明：
     *  %m 消息
     *  %p 日志级别
     *  %r 累计毫秒数
     *  %c 日志名称
     *  %t 线程id
     *  %n 换行
     *  %d 时间
     *  %f 文件名
     *  %l 行号
     *  %T 制表符
     *  %F 协程id
     *  %N 线程名称
     *  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
     */
    using Ptr = std::shared_ptr<LogFormatter>; 

    LogFormatter(const std::string& pattern);

    void reset(const std::string& pattern);

    /**
     * @brief 返回格式化日志文本
     * @param[in] logger 日志器
     * @param[in] level 日志级别
     * @param[in] event 日志事件
     */
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::Ptr event);
    std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::Ptr event);
public:

    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> ptr;

        // 虚析构函数 防止派生类资源不释放
        virtual ~FormatItem() {}

        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::Ptr event) = 0;
        // virtual void format(std::ostream& os, LogEvent::Ptr event) = 0; ?
    };

    void init();

    bool isError() const { return m_error;}

    const std::string getPattern() const { return m_pattern;}
private:
    std::string m_pattern;                  // 日志模式串
    std::vector<FormatItem::ptr> m_items;   // 模式串解析结果
    bool m_error = false;
};

class LogAppender {
friend class Logger;
public:
    using Ptr = std::shared_ptr<LogAppender>;
    using MutexType = std::mutex;

    LogAppender() = default;

    virtual ~LogAppender() = default;

    void setFormatter(LogFormatter::Ptr _formatter);
    LogFormatter::Ptr getFormatter() {return m_formatter;}

    // virtual std::string toYamlString() = 0;

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::Ptr event) = 0;

protected:
    MutexType m_mutex;
    bool m_hasFormatter;
    LogFormatter::Ptr m_formatter;
    LogLevel::Level m_level = LogLevel::DEBUG;
};

class Logger : public std::enable_shared_from_this<Logger> {
public:
    using MutexType = std::mutex;
    using Ptr = std::shared_ptr<Logger>;

    Logger(const std::string& name = "root");
public:
    void log(LogLevel::Level level, LogEvent::Ptr event);

    void debug(LogEvent::Ptr event);

    void info(LogEvent::Ptr event);

    void warn(LogEvent::Ptr event);

    void error(LogEvent::Ptr event);

    void fatal(LogEvent::Ptr event);

    void addAppender(LogAppender::Ptr appender);

    void delAppender(LogAppender::Ptr appender);

    void clearAppenders();

    LogLevel::Level getLevel() const { return m_level;}

    void setLevel(LogLevel::Level val) { m_level = val;}

    const std::string& getName() const { return m_name;}

    void setFormatter(LogFormatter::Ptr val);

    void setFormatter(const std::string& val);

    LogFormatter::Ptr getFormatter();

    std::string toYamlString();

    Logger::Ptr m_root;
private:
    std::string m_name;
    LogLevel::Level m_level;
    MutexType m_mutex;
    std::list<LogAppender::Ptr> m_appenders;
    LogFormatter::Ptr m_formatter;
};

class StdoutLogAppender : public LogAppender {
public:
    using Ptr = std::shared_ptr<StdoutLogAppender>;

    void log(Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override;

    // std::string toYamlString() override;
};

class FileLogAppender : public LogAppender {
public:
    using Ptr = std::shared_ptr<FileLogAppender>;
    FileLogAppender(const std::string& filename);
    void log(Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override;

    bool reopen();
private:
    std::string m_filepath;
    std::ofstream m_filestream;
    uint64_t m_lastTime = 0;
};

class LoggerManager {
public:
    using Ptr = std::shared_ptr<LoggerManager>;

    Logger::Ptr addLogger(const std::string& name);

    Logger::Ptr getRoot() const { return m_root;}

    LoggerManager();

    void init(){}
private:
    Logger::Ptr m_root;
    std::unordered_map<std::string, Logger::Ptr> m_loggers;
};

using LoggerMgr = Singleton<LoggerManager>;

} // namespace sylar


#endif //_SYLAR_LOGGER_H