#include <json/json.h>
#include <string>
#include "http/http_server.h"
#include "http/http_connection.h"
#include "eventpoller/eventpoller.h"

const std::string llm_model = "deepseek-r1";
#define XX(...) #__VA_ARGS__

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
        }
    }
    return 0;
}

void run() {
    sylar::http::HttpServer::ptr server(new sylar::http::HttpServer(true));
    sylar::Address::Ptr addr = sylar::Address::LookupAnyIPAddress("0.0.0.0:8088");
    while(!server->bind(addr)) {
        sleep(2);
    }
    auto sd = server->getServletDispatch();
    sd->addServlet("/generate", generate);
    server->start();
}

int main() {
    sylar::EventPoller ep;
    ep.schedule(run);
    return 0;
}