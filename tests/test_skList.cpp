#include "skipList/skipList.hpp"
#include <string>
#include <iostream>
#include <memory>

int main(int argc, char** argv) {
    sylar::SkipList<std::string, std::string> sl(10);
    sl.insert("1", std::string("1"));
    sl.insert("2", std::string("2"));
    std::string tmp;
    if(!sl.searchValue("1", tmp)) std::cout << "item 1 not found\n";
    else std::cout << tmp << '\n';
    sl.dump();
    sl.erase("1");
    sl.dump();
    return 0;
}