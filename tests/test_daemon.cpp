#include "util/daemon.h"
#include "timer/timer.h"
#include "eventpoller/eventpoller.h"

int run(int argc, char** argv) {
    sylar::EventPoller ep;
    auto timer = ep.addTimer(1000, [](){
        static int cnt = 0;
        std::cout << "child tickle " << cnt << '\n';
        ++cnt;
        if(cnt > 10) {
            exit(1);
        }        
    }, true);
}

int main(int argc, char** argv) {
    return sylar::start_daemon(argc, argv, run, true);
}