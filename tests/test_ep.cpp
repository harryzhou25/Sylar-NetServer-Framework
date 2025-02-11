#include "iomanager/eventpoller.h"
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
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "110.242.68.4", &addr.sin_addr.s_addr);

    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
    } else if(errno == EINPROGRESS) {
        sylar::EventPoller::getThis()->addEvent(sock, sylar::EventPoller::READ, [](){
            Log_Info(Root_Logger()) << "read callback";
        });
        sylar::EventPoller::getThis()->addEvent(sock, sylar::EventPoller::WRITE, [sock](){
            Log_Info(Root_Logger()) << "write callback";
            sylar::EventPoller::getThis()->cancelEvent(sock, sylar::EventPoller::READ);
            close(sock);
        });
    } else {
        Log_Info(Root_Logger()) << "else " << errno << " " << strerror(errno);
    }
}

int main() {
    sylar::EventPoller _ep;
    _ep.schedule(&test);

    return 0;
}