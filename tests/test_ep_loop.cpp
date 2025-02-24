#include "eventpoller/eventpoller.h"
#include "fiber/scheduler.h"
#include "fiber/fiber.h"
#include "log/logger.h"

#include <memory>
#include <iostream>
#include <functional>

// static sylar::Logger::Ptr r_logger = Root_Logger();

class task : public std::enable_shared_from_this<task>{
public:
    using Ptr = std::shared_ptr<task>;
    task(sylar::EventPoller* worker = sylar::EventPoller::getThis(),
        sylar::EventPoller* scheduler = sylar::EventPoller::getThis());
    ~task() = default;
    void start();
    void run();
private:
    sylar::EventPoller* m_worker;
    sylar::EventPoller* m_scheduler;
};

task::task(sylar::EventPoller* worker, sylar::EventPoller* scheduler) 
        : m_worker(worker), m_scheduler(scheduler){}

void task::start() {
    auto self = shared_from_this();
    m_scheduler->schedule([self]() {
        self->run();
    });
}

void task::run() {
    auto self = shared_from_this();
    m_worker->schedule([self](){
        std::cout << "hello\n";
    });
}

void test() {
    task::Ptr tsk(new task);
    tsk->start();
}

int main() {
    sylar::EventPoller ep(2);
    ep.schedule(test);
    return 0;
}