#include "http/http_server.h"
#include "log/logger.h"
#include "../samples/mqServer/mqServer.hpp"

#define XX(...) #__VA_ARGS__

static sylar::messageQueue<std::string> m_mq;

int32_t handlePush(sylar::http::HttpRequest::ptr req,
                    sylar::http::HttpResponse::ptr rsp,
                    sylar::http::HttpSession::ptr session) {
    Json::CharReaderBuilder builder;
    std::string errs;

    std::string req_body = req->getBody();
    std::istringstream req_body_ss(req_body);
    Json::Value tsk_root;
    std::string tsk_str;
    bool res = Json::parseFromStream(builder, req_body_ss, &tsk_root, &errs);
    if(res && tsk_root.isMember("task")) {
        rsp->setStatus(sylar::http::HttpStatus::OK);
    }
    else {
        rsp->setStatus(sylar::http::HttpStatus::BAD_REQUEST);
    }
    return 0;
}

// sylar::EventPoller::Ptr worker;
void run() {
    //sylar::http::HttpServer::ptr server(new sylar::http::HttpServer(true, worker.get(), sylar::IOManager::GetThis()));
    sylar::http::HttpServer::ptr server(new sylar::http::HttpServer(true));

    auto sd = server->getServletDispatch();
    sd->addServlet("/sylar/xx", [](sylar::http::HttpRequest::ptr req
                ,sylar::http::HttpResponse::ptr rsp
                ,sylar::http::HttpSession::ptr session) {
            if(req->getMethod() == sylar::http::HttpMethod::GET) {
                rsp->setBody(req->toString());
            }
            else {
                rsp->setStatus(sylar::http::HttpStatus::BAD_REQUEST);
                rsp->setBody("empty");
            }
            return 0;
    });

    sd->addServlet("/sylar/push", [](sylar::http::HttpRequest::ptr req
                                    ,sylar::http::HttpResponse::ptr rsp
                                    ,sylar::http::HttpSession::ptr session) {
        rsp->setBody(req->toString());
        return 0;
    });

    sd->addGlobServlet("/sylar/*", [](sylar::http::HttpRequest::ptr req
                ,sylar::http::HttpResponse::ptr rsp
                ,sylar::http::HttpSession::ptr session) {
            rsp->setBody("Glob:\r\n" + req->toString());
            return 0;
    });

    sd->addGlobServlet("/sylarx/*", [](sylar::http::HttpRequest::ptr req
                ,sylar::http::HttpResponse::ptr rsp
                ,sylar::http::HttpSession::ptr session) {
            rsp->setBody(XX(<html>
<head><title>404 Not Found</title></head>
<body>
<center><h1>404 Not Found</h1></center>
<hr><center>nginx/1.16.0</center>
</body>
</html>
<!-- a padding to disable MSIE and Chrome friendly error page -->
<!-- a padding to disable MSIE and Chrome friendly error page -->
<!-- a padding to disable MSIE and Chrome friendly error page -->
<!-- a padding to disable MSIE and Chrome friendly error page -->
<!-- a padding to disable MSIE and Chrome friendly error page -->
<!-- a padding to disable MSIE and Chrome friendly error page -->
));
            return 0;
    });

    sylar::Address::Ptr addr = sylar::Address::LookupAnyIPAddress("0.0.0.0:8020");
    while(!server->bind(addr)) {
        sleep(2);
    }

    server->start();
}

int main(int argc, char** argv) {
    sylar::EventPoller iom(1, true, "main");
    // worker.reset(new sylar::EventPoller(3, false, "worker"));
    iom.schedule(run);
    return 0;
}
