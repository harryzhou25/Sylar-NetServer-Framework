#include "eventpoller/eventpoller.h"
#include <iostream>
void run() {
    // for(volatile int i = 0; i < 1000000000; ++i);
    std::cout << "-------- task finished -------- \n";
}

int main(int argc, char** argv) {
    sylar::Manager mgr(3);
    mgr.start();
    while(true) {
        mgr.assignTask(run);
        mgr.assignTask(run);
        sleep(1);
    }
    return 0;
}