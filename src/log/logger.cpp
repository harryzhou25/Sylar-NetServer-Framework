#include "logger.h"

namespace sylar {

const char* LogLevel::ToString(LogLevel::Level level) {
#define level_case(name) \
    case LogLevel::name: \
        return #name; \
        break;
    switch(level) {
        level_case(DEBUG);
        level_case(INFO);
        level_case(WARN);
        level_case(ERROR);
        level_case(FATAL);
        default:
            return "UNKNOW";
    }
#undef level_case
    return "UNKNOWN";
}

LogLevel::Level LogLevel::FromString(const std::string& str) {
#define level_case(event, val_up, val_lo) \
    if(str == #val_up || str == #val_lo) { \
        return LogLevel::event; \
    }
    level_case(DEBUG, "DEBUG", "debug");
    level_case(INFO, "INFO", "info");
    level_case(WARN, "warn", "warn");
    level_case(ERROR, "ERROR", "error");
    level_case(FATAL, "FATAL", "fatal");
#undef level_case
    return LogLevel::UNKNOW;
}

LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
            ,const char* file, int32_t line, uint32_t elapse
            ,uint32_t thread_id, uint32_t fiber_id, uint64_t time
            ,const std::string& thread_name)
    :m_file(file)
    ,m_line(line)
    ,m_elapse(elapse)
    ,m_threadId(thread_id)
    ,m_fiberId(fiber_id)
    ,m_time(time)
    ,m_threadName(thread_name)
    ,m_logger(logger)
    ,m_level(level) {
        
}

// ???
void LogEvent::format(const char* fmt, ...) {
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}

// ???
void LogEvent::format(const char* fmt, va_list al) {
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if(len != -1) {
        m_ss << std::string(buf, len);
        free(buf);
    }
}

LogEventWrap::LogEventWrap(LogEvent::Ptr e) {
    m_event = std::move(e);
}

LogEventWrap::~LogEventWrap() {
    m_event->getLogger()->log(m_event->getLevel(), m_event);
}

std::stringstream& LogEventWrap::getSS() {
    if(m_event) {
        return m_event->getSS();
    }
    std::stringstream _ss;
    return _ss;
}

////////////////////////   Log Formatters  //////////////////////////////

class MessageFormatItem : public LogFormatter::FormatItem {
public:
    MessageFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override {
        os << event->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem {
public:
    LevelFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override {
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem {
public:
    ElapseFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override {
        os << event->getElapse();
    }
};

class NameFormatItem : public LogFormatter::FormatItem {
public:
    NameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override {
        os << event->getLogger()->getName();
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem {
public:
    ThreadIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override {
        os << event->getThreadId();
    }
};

class FiberIdFormatItem : public LogFormatter::FormatItem {
public:
    FiberIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override {
        os << event->getFiberId();
    }
};

class ThreadNameFormatItem : public LogFormatter::FormatItem {
public:
    ThreadNameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override {
        os << event->getThreadName();
    }
};

class DateTimeFormatItem : public LogFormatter::FormatItem {
public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
        :m_format(format) {
        if(m_format.empty()) {
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }

    void format(std::ostream& os, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }
private:
    std::string m_format;
};

class FilenameFormatItem : public LogFormatter::FormatItem {
public:
    FilenameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override {
        os << event->getFile();
    }
};

class LineFormatItem : public LogFormatter::FormatItem {
public:
    LineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override {
        os << event->getLine();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem {
public:
    NewLineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override {
        os << std::endl;
    }
};

class StringFormatItem : public LogFormatter::FormatItem {
public:
    StringFormatItem(const std::string& str)
        :m_string(str) {}
    void format(std::ostream& os, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override {
        os << m_string;
    }
private:
    std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem {
public:
    TabFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override {
        os << "\t";
    }
private:
    std::string m_string;
};

LogFormatter::LogFormatter(const std::string& pattern)
    :m_pattern(pattern) {
    init();
}

void LogFormatter::reset(const std::string& pattern) {
    m_pattern = std::move(pattern);
    init();
    std::cout << "[LogFormatter] reset : " << m_pattern << '\n';
}

std::string LogFormatter::format(Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) {
    std::stringstream t_ss;
    for(auto& i : m_items) {
        i->format(t_ss, logger, level, event);
    }
    return t_ss.str();
}

std::ostream& LogFormatter::format(std::ostream& ofs, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) {
    int idx = 0;
    for(auto& i : m_items) {
        i->format(ofs, logger, level, event);
    }
    return ofs;
}

void LogFormatter::init() {
    // str, format, type
    // type   - 0 : 一般字符串
    //        - 1 : 日志内容
    std::vector<std::tuple<std::string, std::string, int> > vec;
    std::string nstr;
    for(size_t i = 0; i < m_pattern.size(); ++i) {
        if(m_pattern[i] != '%') {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        if((i + 1) < m_pattern.size()) {
            if(m_pattern[i + 1] == '%') {
                nstr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < m_pattern.size()) {
            if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                    && m_pattern[n] != '}')) {
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }
            if(fmt_status == 0) {
                if(m_pattern[n] == '{') {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    //std::cout << "*" << str << std::endl;
                    fmt_status = 1; //解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            }
            else if(fmt_status == 1) {
                if(m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    //std::cout << "#" << fmt << std::endl;
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if(n == m_pattern.size()) {
                if(str.empty()) {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if(fmt_status == 0) {
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } 
        else if(fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }

    if(!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }
    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
#define mapItem(str, C) \
        {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}
        mapItem(m, MessageFormatItem),           //m:消息
        mapItem(p, LevelFormatItem),             //p:日志级别
        mapItem(r, ElapseFormatItem),            //r:累计毫秒数
        mapItem(c, NameFormatItem),              //c:日志名称
        mapItem(t, ThreadIdFormatItem),          //t:线程id
        mapItem(n, NewLineFormatItem),           //n:换行
        mapItem(d, DateTimeFormatItem),          //d:时间
        mapItem(f, FilenameFormatItem),          //f:文件名
        mapItem(l, LineFormatItem),              //l:行号
        mapItem(T, TabFormatItem),               //T:Tab
        mapItem(F, FiberIdFormatItem),           //F:协程id
        mapItem(N, ThreadNameFormatItem),        //N:线程名称
#undef mapItem
    };

    for(auto& i : vec) {
        if(std::get<2>(i) == 0) {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        } 
        else {
            auto it = s_format_items.find(std::get<0>(i));
            if(it == s_format_items.end()) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                m_error = true;
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
    }
}

/////////////////// Log Appenders /////////////////////

void LogAppender::setFormatter(LogFormatter::Ptr val) {
    m_formatter = val;
    if(m_formatter) {
        m_hasFormatter = true;
    } 
    else {
        m_hasFormatter = false;
    }
}

void StdoutLogAppender::log(Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) {
    if(m_formatter && level >= m_level) {
        m_formatter->format(std::cout, logger, level, event);
    }
}

FileLogAppender::FileLogAppender(const std::string& filepath){
    m_filepath = std::move(filepath);
    filePathCheck(m_filepath);
    m_filestream.open(m_filepath, std::ios::app);
}

bool FileLogAppender::reopen() {
    filePathCheck(m_filepath);
    m_filestream.open(m_filepath, std::ios::app);
}

void FileLogAppender::log(Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) {
    if(!m_filestream.is_open()) reopen();
    m_formatter->format(m_filestream, logger, level, event);
}

/////////////////// Logger /////////////////////

Logger::Logger(const std::string& name) {
    m_name = std::move(name);
    m_level = LogLevel::DEBUG;
    m_formatter = std::make_shared<LogFormatter>(DEFAULT_FORMAT);
}

void Logger::setFormatter(LogFormatter::Ptr val) {
    m_formatter = val;
    for(auto& i : m_appenders) {
        if(!i->m_hasFormatter) {
            i->m_formatter = m_formatter;
        }
    }
}

void Logger::setFormatter(const std::string& val) {
    std::cout << "---" << val << std::endl;
    LogFormatter::Ptr new_formatter = std::make_shared<LogFormatter>(val);
    if(new_formatter->isError()) {
        std::cout << "Logger setFormatter name=" << m_name
                  << " value=" << val << " invalid formatter"
                  << std::endl;
        return;
    }
    //m_formatter = new_val;
    setFormatter(new_formatter);
}

LogFormatter::Ptr Logger::getFormatter() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_formatter;
}

void Logger::addAppender(LogAppender::Ptr appender) {
    if(!appender->getFormatter()) {
        appender->m_formatter = m_formatter;
    }
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::Ptr appender) {
    for(auto it = m_appenders.begin();
            it != m_appenders.end(); ++it) {
        if(*it == appender) {
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::clearAppenders() {
    m_appenders.clear();
}

void Logger::log(LogLevel::Level level, LogEvent::Ptr event) {
    if(level >= m_level) {
        auto self = shared_from_this();
        // MutexType::Lock lock(m_mutex);
        if(!m_appenders.empty()) {
            for(auto& i : m_appenders) {
                i->log(self, level, event);
            }
        } 
        else if(m_root) {
            m_root->log(level, event);
        }
    }
}

void Logger::debug(LogEvent::Ptr event) {
    log(LogLevel::DEBUG, event);
}

void Logger::info(LogEvent::Ptr event) {
    log(LogLevel::INFO, event);
}

void Logger::warn(LogEvent::Ptr event) {
    log(LogLevel::WARN, event);
}

void Logger::error(LogEvent::Ptr event) {
    log(LogLevel::ERROR, event);
}

void Logger::fatal(LogEvent::Ptr event) {
    log(LogLevel::FATAL, event);
}

} // namespace sylar
