#include "skipList/skipList.hpp"
#include <string>
#include <iostream>

int main(int argc, char** argv) {
    sylar::SkipList<std::string, std::string> sl(10);
    for(int i = 0; i <= 5; ++i)
    sl.insert(std::to_string(i), std::to_string(i));
    for(int i = 5; i >= 0; --i) {
        sl.erase(std::to_string(i));
        sl.dump();
    }
    return 0;
}