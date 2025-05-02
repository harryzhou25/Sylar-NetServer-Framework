#ifndef _SYLAR_UTIL_H
#define _SYLAR_UTIL_H

#include <pthread.h>
#include <unistd.h>
#include <syscall.h>
#include <sys/types.h>
#include <stdint.h>
#include <filesystem>
#include <string>
#include <iostream>
#include <vector>
#include <cxxabi.h>
#include <json/json.h>
#include <yaml-cpp/yaml.h>

namespace sylar {
    
pid_t getThreadId();

uint32_t getFiberId();

void filePathCheck(const std::string& path);

void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);

std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "\t");

uint64_t getCurrentMS();

std::string Time2Str(time_t ts = time(0), const std::string& format = "%Y-%m-%d %H:%M:%S");
time_t Str2Time(const char* str, const char* format = "%Y-%m-%d %H:%M:%S");

static uint64_t getTimeUsec() {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
}

bool YamlToJson(const YAML::Node& ynode, Json::Value& jnode);

class noncopyable {
protected:
    noncopyable() {}
    ~noncopyable() {}
private:
    //禁止拷贝  [AUTO-TRANSLATED:e8af72e3]
    //Prohibit copying
    noncopyable(const noncopyable &that) = delete;
    noncopyable(noncopyable &&that) = delete;
    noncopyable &operator=(const noncopyable &that) = delete;
    noncopyable &operator=(noncopyable &&that) = delete;
};

template<class T>
const char* TypeToName() {
    static const char* s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
    return s_name;
}

class FSUtil {
public:
    static void ListAllFile(std::vector<std::string>& files
                            ,const std::string& path
                            ,const std::string& subfix);
    static bool Mkdir(const std::string& dirname);
    static bool IsRunningPidfile(const std::string& pidfile);
    static bool Rm(const std::string& path);
    static bool Mv(const std::string& from, const std::string& to);
    static bool Realpath(const std::string& path, std::string& rpath);
    static bool Symlink(const std::string& frm, const std::string& to);
    static bool Unlink(const std::string& filename, bool exist = false);
    static std::string Dirname(const std::string& filename);
    static std::string Basename(const std::string& filename);
    static bool OpenForRead(std::ifstream& ifs, const std::string& filename
                    ,std::ios_base::openmode mode);
    static bool OpenForWrite(std::ofstream& ofs, const std::string& filename
                    ,std::ios_base::openmode mode);
};

class StringUtil {
    public:
        static std::string Format(const char* fmt, ...);
        static std::string Formatv(const char* fmt, va_list ap);
    
        static std::string UrlEncode(const std::string& str, bool space_as_plus = true);
        static std::string UrlDecode(const std::string& str, bool space_as_plus = true);
    
        static std::string Trim(const std::string& str, const std::string& delimit = " \t\r\n");
        static std::string TrimLeft(const std::string& str, const std::string& delimit = " \t\r\n");
        static std::string TrimRight(const std::string& str, const std::string& delimit = " \t\r\n");
    
    
        static std::string WStringToString(const std::wstring& ws);
        static std::wstring StringToWString(const std::string& s);
    
};

class TypeUtil {
    public:
        static int8_t ToChar(const std::string& str);
        static int64_t Atoi(const std::string& str);
        static double Atof(const std::string& str);
        static int8_t ToChar(const char* str);
        static int64_t Atoi(const char* str);
        static double Atof(const char* str);
};

} // namespace sylar


#endif //_SYLAR_UTIL_H