#include "skipList/skipList.hpp"
#include <string>
#include <iostream>

int main(int argc, char** argv) {
    sylar::SkipList<std::string, std::string> sl(10);
    sl.insert("1", "1");
    sl.insert("2", "2");
    if(sl.search("1")) std::cout << "found item 1\n";
    else std::cout << "not found item 1"<< '\n';
    if(sl.search("2")) std::cout << "found item 2\n";
    else std::cout << "not found item 2"<< '\n';
    sl.erase("1");
    if(sl.search("1")) std::cout << "found item 1\n";
    else std::cout << "not found item 1"<< '\n';
    return 0;
}