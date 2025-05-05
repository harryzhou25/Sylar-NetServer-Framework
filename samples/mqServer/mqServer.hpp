#include "http/http.h"
#include "http/http_server.h"
#include "http/http_session.h"
#include "socket/socket.h"
#include "socket/address.h"
#include "eventpoller/eventpoller.h"
#include "skipList/messageQueue.hpp"

#include <list>
#include <memory>
#include <json/json.h>

namespace sylar {

class mqServer : public std::enable_shared_from_this<mqServer> {
public:
    using Ptr = std::shared_ptr<mqServer>;
public:
mqServer(uint64_t timeoutms, Address::Ptr addr) :m_timeoutms(timeoutms), m_addr(addr){
    m_server = std::shared_ptr<http::HttpServer>(new http::HttpServer(false));
    m_mq = std::shared_ptr<messageQueue<std::string>>(new messageQueue<std::string>);
}

int32_t handlePush(http::HttpRequest::ptr req,
                http::HttpResponse::ptr rsp,
                http::HttpSession::ptr session) {
    Json::CharReaderBuilder builder;
    std::string errs;

    std::string req_body = req->getBody();
    std::istringstream req_body_ss(req_body);
    Json::Value tsk_root;
    std::string tsk_str;
    bool res = Json::parseFromStream(builder, req_body_ss, &tsk_root, &errs);
    if(res && tsk_root.isMember("task")) {
        tsk_str = tsk_root["task"].asString();
        if(tsk_root.isMember("level") && tsk_root["level"].asInt() == 1) {
            m_mq->push_front(tsk_str);
        }
        else {
            m_mq->push_back(tsk_str);
        }
        rsp->setStatus(http::HttpStatus::OK);
    }
    else {
        rsp->setStatus(http::HttpStatus::BAD_REQUEST);
    }
    rsp->setBody("empty body");
    return 0;
}

int32_t handlePull(http::HttpRequest::ptr req,
                    http::HttpResponse::ptr rsp,
                    http::HttpSession::ptr session) {
    Json::Value rsp_root;
    rsp_root["task"] = "";
    rsp_root["empty"] = true;
    if(m_mq->size() > 0) {
        auto tsk = m_mq->pop_front();
        rsp->setClose(false);
        rsp_root["task"] = tsk;
        rsp_root["empty"] = false;
    }
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";
    std::string body_str = Json::writeString(writer, rsp_root);
    rsp->setBody(body_str);
    return 0;
}

void start() {
    m_server->start();
}

void init() {
    while(!m_server->bind(m_addr)) {
        sleep(2);
    }
    auto sd = m_server->getServletDispatch();
    auto self = shared_from_this();
    sd->addServlet("/push", [self](http::HttpRequest::ptr req,
                                    http::HttpResponse::ptr rsp,
                                    http::HttpSession::ptr session) {
        return self->handlePush(req, rsp, session);
        return 0;
    });
    sd->addServlet("/pull", [self](http::HttpRequest::ptr req,
                                    http::HttpResponse::ptr rsp,
                                    http::HttpSession::ptr session) {
        return self->handlePull(req, rsp, session);
        return 0;
    });
}

private:
    uint64_t m_timeoutms;
    Address::Ptr m_addr;
    std::list<http::HttpSession*> m_waitlist;
    std::shared_ptr<http::HttpServer> m_server;
    std::shared_ptr<messageQueue<std::string>> m_mq;
};

} // namespace sylar