#ifndef _SYLAR_UTIL_H
#define _SYLAR_UTIL_H

#include <pthread.h>
#include <unistd.h>
#include <syscall.h>
#include <sys/types.h>
#include <stdint.h>

namespace sylar {
    
pid_t getThreadId();
uint32_t getFiberId();

} // namespace sylar


#endif //_SYLAR_UTIL_H