#include "http/http_server.h"
#include "http/http_connection.h"
#include "eventpoller/eventpoller.h"

#include <string>
#include <chrono>
#include <json/json.h>

const std::string llm_model = "deepseek-r1";

int32_t generate(sylar::http::HttpRequest::ptr req, 
              sylar::http::HttpResponse::ptr rsp,
              sylar::http::HttpSession::ptr session) {
    sylar::http::HttpMethod method = req->getMethod();
    if(method == sylar::http::HttpMethod::POST) {
        Json::CharReaderBuilder builder;
        std::map<std::string, std::string> header;
        Json::Value body;

        std::string errs;
        std::string body_str = req->getBody();
        std::istringstream body_ss(body_str);
        Json::Value user_msg;
        std::string user_msg_str;
        bool ok = Json::parseFromStream(builder, body_ss, &user_msg, &errs);
        if (ok) {
            user_msg_str = user_msg["message"].asString();
        }

        body["model"]  = llm_model;
        body["prompt"] = user_msg_str;
        body["stream"] = false;
        Json::StreamWriterBuilder writer;
        writer["indentation"] = "";
        body_str = Json::writeString(writer, body);

        auto r = sylar::http::HttpConnection::DoPost("http://localhost:11434/api/generate", -1, header, body_str);
        if(r->response) {
            std::cout << "=== " << sylar::http::HttpStatusToString(r->response->getStatus()) << " ===\n";
            if(r->response->getStatus() != sylar::http::HttpStatus::OK) {
                std::cout << "llm error\n";
                std::cout << r->response->toString() << '\n';
                return 0;
            }
            std::istringstream llm_ss(r->response->getBody());
            Json::Value llm_resp;
            ok = Json::parseFromStream(builder, llm_ss, &llm_resp, &errs);
            if(ok) {
                std::string model_resp = llm_resp["response"].asString();
                rsp->setBody(model_resp);
            }
        }
        else {
            std::cout << "=== " << "Response empty" << " ===\n";
            rsp->setBody("");
        }
    }
    return 0;
}

void run() {
    int cnt = 0;
    int cnt_mx = 2;
    while(cnt <= cnt_mx) {
        bool sleep_flag = false;
        std::map<std::string, std::string> header;
        std::string body_str;
        auto r = sylar::http::HttpConnection::DoGet("http://localhost:8042/pull", 100, header, "");
        std::string jsonStr = r->response->getBody();
        std::istringstream json_ss(jsonStr);
        Json::CharReaderBuilder builder;
        Json::Value root;
        std::string errs;
        bool ok = Json::parseFromStream(builder, json_ss, &root, &errs);
        if(ok) {
            if(!root["empty"].asBool()) {
                std::string task = root["task"].asString();
                std::cout << "Pulled task: " << task << '\n';
                Json::Value body;
                body["model"]  = llm_model;
                body["prompt"] = task;
                body["stream"] = false;
                Json::StreamWriterBuilder writer;
                writer["indentation"] = "";
                body_str = Json::writeString(writer, body);
                auto model_rsp = sylar::http::HttpConnection::DoPost("http://localhost:11434/api/generate", -1, header, body_str);
                if(r->response->getStatus() != sylar::http::HttpStatus::OK) {
                    sleep_flag = true;
                }
                else {
                    std::istringstream llm_ss(r->response->getBody());
                    Json::Value llm_resp;
                    bool model_ok = Json::parseFromStream(builder, llm_ss, &llm_resp, &errs);
                    if(model_ok) {
                        std::string model_resp = llm_resp["response"].asString();
                        std::cout << model_resp << '\n';
                    }
                    else {
                        sleep_flag = true;
                    }
                }
            }
            else {
                sleep_flag = true;
            }
        }
        else {
            sleep_flag = true;
        }
        if(sleep_flag) {
            std::cout << "start sleep\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        ++cnt;
    }
}

int main() {
    sylar::EventPoller ep;
    ep.schedule(run);
    return 0;
}