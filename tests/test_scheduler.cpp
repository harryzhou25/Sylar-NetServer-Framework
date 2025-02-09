#include "fiber/scheduler.h"

int main() {
    sylar::Scheduler sc;
    sc.start();
    std::cout << "-----------\n";
    sc.stop();
    return 0;
}