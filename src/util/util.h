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

namespace sylar {
    
pid_t getThreadId();

uint32_t getFiberId();

void filePathCheck(const std::string& path);

} // namespace sylar


#endif //_SYLAR_UTIL_H