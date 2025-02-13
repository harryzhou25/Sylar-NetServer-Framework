#include "eventpoller/eventpoller.h"
#include "fiber/scheduler.h"
#include "fiber/fiber.h"
#include "log/logger.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void test() {
    Log_Info(Root_Logger()) << "timer";
}

int main() {
    sylar::EventPoller _ep;
    _ep.addTimer(500, &test, true);
    return 0;
}