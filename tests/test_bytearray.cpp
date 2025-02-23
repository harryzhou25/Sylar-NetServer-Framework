#include "socket/bytearray.h"
#include <iostream>

int main() {

    sylar::ByteArray bt(1024);
    int8_t x = -1;
    uint8_t _x = 125;
    bt.writeFuint8(_x);
    bt.setPosition(0);
    std::cout << _x << ':' << bt.readFuint8() << '\n';
    return 0;
}