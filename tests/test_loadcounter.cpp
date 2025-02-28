#include "eventpoller/eventpoller.h"
#include "log/logger.h"

void run() {
    std::cout << "Thread load: " << sylar::EventPoller::getThis()->getLoad() << '\n';
    for(volatile int i = 1; i <= 1000000000; ++i);
    sleep(2);
}

int main(int argc, char** argv) {
    sylar::EventPoller ep(2);
    std::cout << sylar::is_hook_enable() << '\n';
    ep.schedule(run);
    return 0;
}