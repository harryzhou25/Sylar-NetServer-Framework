#include "util/hook.h"
#include "log/logger.h"
#include "eventpoller/eventpoller.h"

int main() {
    sylar::EventPoller ep;
    ep.schedule([] {
        sleep(2);
        Log_Debug(Root_Logger()) << "sleep 2";
    });
    ep.schedule([] {
        sleep(3);
        Log_Debug(Root_Logger()) << "sleep 3";
    });
    return 0;
}