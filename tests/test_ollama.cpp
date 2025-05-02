#include <iostream>
#include <fstream>
#include <map>
#include <string>

#include "http/http_connection.h"
#include "http/http_parser.h"

int main() {
    std::map<std::string, std::string> header;
    auto r = sylar::http::HttpConnection::DoPost("http://localhost:11434/api/generate", 300, header,
    R"({
        "model": "deepseek-r1",
        "prompt": "Who is Kimi Raikkonen.",
        "stream": false
    })");
    sylar::http::HttpResponseParser parser;
    std::string jsonStr = r->response->getBody();
    std::istringstream json_ss(jsonStr);
    Json::CharReaderBuilder builder;
    Json::Value root;
    std::string errs;
    bool ok = Json::parseFromStream(builder, json_ss, &root, &errs);
    if(ok) {
        std::cout << root["model"] << '\n';
        std::cout << root["response"] << '\n';
    }
    return 0;
}