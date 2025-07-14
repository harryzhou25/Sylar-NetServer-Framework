#include "zkClient/zkClient.h"
#include <iostream>
#include <unistd.h>
#include <vector>

void on_watcher(int type, int stat, const std::string& path, sylar::zkClient::ptr client) {
    std::cout << "received event type: " << type << ',' << stat << '\n';
    if(stat == ZOO_CONNECTED_STATE) {
        std::cout << "connected server\n";
    }
    if(!path.empty()) {
        std::cout << " path: " << path << '\n';
    }
}

int main() {
    sylar::zkClient::Ptr zk(new sylar::zkClient);
    std::cout << zk->init("127.0.0.1:2181", 3000, on_watcher) << '\n';
    usleep(100 * 1000);
    std::string path = "/echo";
    std::vector<std::string> res;
    zk->getChildren(path, res, 0);

    std::cout << res.size() << '\n';
    for(auto i : res) {
        std::string node = path + '/' + i;
        std::string data;
        data.resize(512);
        zk->get(node, data, false);
        std::cout << node << " -> " << data << '\n';
    }

    return 0;
}