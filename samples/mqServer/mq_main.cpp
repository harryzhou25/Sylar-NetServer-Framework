#include <string>
#include <json/json.h>
#include "../samples/mqServer/mqServer.hpp"
#include "http/http_connection.h"
#include "eventpoller/eventpoller.h"

void run() {
    sylar::Address::Ptr addr = sylar::Address::LookupAnyIPAddress("0.0.0.0:8042");
    std::shared_ptr<sylar::mqServer> mq(new sylar::mqServer(300, addr));
    mq->init();
    mq->start();
}

int main(int argc, char** argv) {
    sylar::EventPoller ep(4);
    ep.schedule(run);
}