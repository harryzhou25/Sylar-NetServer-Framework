#include "skipList/messageQueue.hpp"

#include <iostream>

int main() {
    sylar::messageQueue<std::string> msg_q("chat");
    msg_q.push_back(std::string("Hi"));
    msg_q.push_back(std::string("Hello"));
    std::cout << msg_q.front() << '\n';
    msg_q.pop_front();
    std::cout << msg_q.front() << '\n';
    return 0;
}