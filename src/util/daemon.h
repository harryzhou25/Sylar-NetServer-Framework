#ifndef _SYLAR_DAEMON_H_
#define _SYLAR_DAEMON_H_

#include <cstdint>
#include <unistd.h>
#include <functional>

#include "util/Singleton.h"

namespace sylar {
    struct ProcessInfo {
        pid_t parend_id = 0;
        pid_t main_id = 0;
        uint64_t restart_cnt = 0;
        uint64_t parend_start_time = 0;
        uint64_t main_start_time = 0;
    };

    using ProcessInfoMgr = Singleton<ProcessInfo>;

    int start_daemon(int argc, char** argv, 
                    std::function<int(int argc, char** argv)> main_cb,
                    bool is_deamon);
} // namespace sylar


#endif //_SYLAR_DAEMON_H_