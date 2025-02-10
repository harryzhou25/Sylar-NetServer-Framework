#include "fiber/scheduler.h"
#include "fiber/fiber.h"
#include "log/logger.h"

void test() {
    Log_Debug(Root_Logger()) << "Fiber test";
}

int main() {
    sylar::Scheduler sc(3, true, "test");
    sc.schedule(&test);
    sc.start();
    Log_Debug(Root_Logger()) << "going to schedule";
    sc.stop();
    Log_Debug(Root_Logger()) << "end";
    return 0;
}