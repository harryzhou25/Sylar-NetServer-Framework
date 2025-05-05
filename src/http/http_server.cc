#include "http/http_server.h"
#include "log/logger.h"
#include "http/servlets/config_servlet.h"
// #include "http/servlets/status_servlet.h"

namespace sylar {
namespace http {

static sylar::Logger::Ptr g_logger = Name_Logger("system");

HttpServer::HttpServer(bool keepalive
               ,sylar::EventPoller* worker
               ,sylar::EventPoller* io_worker
               ,sylar::EventPoller* accept_worker)
    :TcpServer(worker, io_worker, accept_worker)
    ,m_isKeepalive(keepalive) {
    m_dispatch.reset(new ServletDispatch);

    // m_type = "http";
    // m_dispatch->addServlet("/_/status", Servlet::ptr(new StatusServlet));
    m_dispatch->addServlet("/_/config", Servlet::ptr(new ConfigServlet));
}

void HttpServer::setName(const std::string& v) {
    TcpServer::setName(v);
    m_dispatch->setDefault(std::make_shared<NotFoundServlet>(v));
}

void HttpServer::handleClient(Socket::Ptr client) {
    HttpSession::ptr session(new HttpSession(client));
    do {
        // std::cout << "HttpServer::handleClient\n";
        auto req = session->recvRequest();
        if(!req) {
            // Log_Debug(g_logger) << "recv http request fail, errno="
            //     << errno << " errstr=" << strerror(errno)
            //     << " cliet:" << *client << " keep_alive=" << m_isKeepalive;
            break;
        }

        HttpResponse::ptr rsp(new HttpResponse(req->getVersion()
                            ,req->isClose() || !m_isKeepalive));
        rsp->setHeader("Server", getName());
        m_dispatch->handle(req, rsp, session);
        session->sendResponse(rsp);

        if(!m_isKeepalive || req->isClose()) {
            break;
        }
    } while(true);
    session->close();
}
}

}
