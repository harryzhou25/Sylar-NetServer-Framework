#include "util/daemon.h"
#include "log/logger.h"

#include <sys/wait.h>
#include <string.h>

namespace sylar {
    static Logger::Ptr g_logger = Name_Logger("system");

    static uint32_t restart_timeout = 5;

    int run(int argc, char** argv, std::function<int(int argc, char** argv)> main_cb) {
        ProcessInfoMgr::getInstance()->main_id = getpid();
        ProcessInfoMgr::getInstance()->main_start_time = time(0);
        main_cb(argc, argv);
    }

    int run_daemon(int argc, char** argv, std::function<int(int argc, char** argv)> main_cb) {
        Log_Debug(g_logger) << "parent pid: " << getpid();
        daemon(1, 0);
        ProcessInfoMgr::getInstance()->parend_id = getpid();
        ProcessInfoMgr::getInstance()->parend_start_time = time(0);
        while(true) {
            auto pid = fork();
            if(pid == 0) {
                Log_Info(g_logger) << "main process running: " << getpid();
                return run(argc, argv, main_cb);                
            }
            else if(pid < 0) {
                Log_Error(g_logger) << "fork fail return=" << pid
                << " errno=" << errno << " errstr=" << strerror(errno);
                break;
            }
            else {
                int status = 0;
                Log_Info(g_logger) << "parent process: " << getpid() << "forked: " << pid;
                waitpid(pid, &status, 0);
                if(status == 9) {
                    Log_Info(g_logger) << "child process killed " << status;
                    break;
                }
                else {
                    Log_Info(g_logger) << "child process crashed: " << status;
                    break;
                }
                ++ProcessInfoMgr::getInstance()->restart_cnt;
                sleep(restart_timeout);
            }
        }
        return 0;
    }

    int start_daemon(int argc, char** argv, 
        std::function<int(int argc, char** argv)> main_cb,
        bool is_daemon) {
            if(!is_daemon) {
                ProcessInfoMgr::getInstance()->parend_id = getpid();
                ProcessInfoMgr::getInstance()->parend_start_time = time(0);
                return run(argc, argv, main_cb);
            }
            return run_daemon(argc, argv, main_cb);
    }
} // namespace sylar
