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
#include <yaml-cpp/yaml.h>

namespace sylar {
    
pid_t getThreadId();

uint32_t getFiberId();

void filePathCheck(const std::string& path);

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

} // namespace sylar


#endif //_SYLAR_UTIL_H