#include "timer/timer.h"
#include "util/util.h"
#include "util/ConcurrentVector.h"
#include "eventpoller/eventpoller.h"
sylar::ConcurrentVector<int>::Ptr vec(new sylar::ConcurrentVector<int>);

int main(int argc, char** argv) {
    sylar::EventPoller ep(3);
    vec->push_back(1);
    vec->push_back(2);
    ep.addTimer(500, [](){
        sylar::ConcurrentVector<int>::Ptr _vec = vec;
        (*_vec).set(0, sylar::getThreadId());
        std::cout << sylar::getThreadId() <<  " ----- operated ----- \n";
    }, true);
    ep.addTimer(500, [](){
        std::cout << "----- ConcurrentVector size: " << vec->size() << " -----" << '\n';
        auto sz = vec->size();
        for(int i = 0; i < sz; ++i) {
            std::cout << (*vec)[i] << ' ';
        }
        std::cout << '\n';
    }, true);
    return 0;
}