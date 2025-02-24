#include "socket/socket.h"
#include "log/logger.h"
#include "util/macro.h"
#include "util/hook.h"
#include "eventpoller/eventpoller.h"
#include "socket/fdManager.h"

#include <netinet/tcp.h>

namespace sylar {
static Logger::Ptr g_logger = Name_Logger("system");

Socket::Ptr Socket::CreateTCP(Address::Ptr address){
    Socket::Ptr sock(new Socket(address->getFamily(), TCP, 0));
    return sock;
}

Socket::Ptr Socket::CreateUDP(Address::Ptr address){
    Socket::Ptr sock(new Socket(address->getFamily(), UDP, 0));
    return sock;
}

Socket::Ptr Socket::CreateTCPSocket(){
    Socket::Ptr sock(new Socket(IPv4, TCP, 0));
    return sock;
}

Socket::Ptr Socket::CreateUDPSocket(){
    Socket::Ptr sock(new Socket(IPv4, UDP, 0));
    return sock;
}

Socket::Ptr Socket::CreateTCPSocket6(){
    Socket::Ptr sock(new Socket(IPv6, TCP, 0));
    return sock;
}

Socket::Ptr Socket::CreateUDPSocket6(){
    Socket::Ptr sock(new Socket(IPv6, UDP, 0));
    return sock;
}

Socket::Ptr Socket::CreateUnixTCPSocket(){
    Socket::Ptr sock(new Socket(UNIX, TCP, 0));
    return sock;
}

Socket::Ptr Socket::CreateUnixUDPSocket(){
    Socket::Ptr sock(new Socket(UNIX, UDP, 0));
    return sock;
}

Socket::Socket(int family, int type, int protocol):
    m_family(family), m_type(type), m_protocol(protocol) {
    m_sock = -1;
    m_Connected = false;
}

Socket::~Socket() {
    close();
    if(m_remoteAddress && m_localAddress) {
        Log_Debug(g_logger) << "~Socket" << m_localAddress->toString() << ' ' << m_remoteAddress->toString();
    }
}

int64_t Socket::getSendTimeout() {
    auto ctx = FdMgr::getInstance()->get(m_sock);
    if(ctx) {
        return ctx->getTimeout(SO_SNDTIMEO);
    }
    return -1;
}

void Socket::setSendTimeout(int64_t v) {
    auto ctx = FdMgr::getInstance()->get(m_sock);
    if(ctx) {
        ctx->setTimeout(SO_SNDTIMEO, v);
    }
}

int64_t Socket::getRecvTimeout() {
    auto ctx = FdMgr::getInstance()->get(m_sock);
    if(ctx) {
        return ctx->getTimeout(SO_RCVTIMEO);
    }
    return -1;
}

void Socket::setRecvTimeout(int64_t v) {
    auto ctx = FdMgr::getInstance()->get(m_sock);
    if(ctx) {
        ctx->setTimeout(SO_RCVTIMEO, v);
    }
}

bool Socket::getOption(int level, int option, void* result, socklen_t* len) {
    if(getsockopt(m_sock, level, option, result, (socklen_t*)len)) {
        Log_Error(g_logger) << "getOption sock=" << m_sock
            << " level=" << level << " option=" << option
            << " errno=" << errno << " errstr=" << strerror(errno);
        return false;
    }
    return true;
}

bool Socket::setOption(int level, int option, const void* result, socklen_t len) {
    if(setsockopt(m_sock, level, option, result, (socklen_t)len)) {
        Log_Error(g_logger) << "setOption sock=" << m_sock
            << " level=" << level << " option=" << option
            << " errno=" << errno << " errstr=" << strerror(errno);
        return false;
    }
    return true;
}

bool Socket::init(int sock) {
    auto ctx = FdMgr::getInstance()->get(sock);
    if(ctx && ctx->isSocket() && !ctx->isClose()) {
        m_sock = sock;
        m_Connected = true;
        initSock();
        getLocalAddress();
        getRemoteAddress();
        return true;
    }
    return false;
}

void Socket::newSock() {
    m_sock = socket(m_family, m_type, m_protocol);
    if(UNLIKELY(m_sock == -1)) {
        initSock();
    }
    else {
        Log_Error(g_logger) << "socket(" << m_family
        << ", " << m_type << ", " << m_protocol << ") errno="
        << errno << " errstr=" << strerror(errno);
    }
}

void Socket::initSock() {
    int val = 1;
    setOption(SOL_SOCKET, SO_REUSEADDR, val);
    if(m_type == SOCK_STREAM) {
        setOption(IPPROTO_TCP, TCP_NODELAY, val);
    }
}

Socket::Ptr Socket::accept() {
    // std::cout << m_sock << '\n';
    int sockFd = ::accept(m_sock, nullptr, nullptr);
    if(sockFd == -1) {
        Log_Error(g_logger) << "accept(" << m_sock << ") errno="
            << errno << " errstr=" << strerror(errno);
        return nullptr;
    }
    Socket::Ptr sock(new Socket(m_family, m_type, m_protocol));
    if(sock->init(sockFd)) {
        return sock;
    }
    return nullptr;
}

bool Socket::bind(const Address::Ptr addr) {
    if(!isValid()) {
        newSock();
        if(!isValid()) {
            return false;
        }
    }
    
    if(UNLIKELY(addr->getFamily() != m_family)) {
        Log_Error(g_logger) << "bind sock.family("
        << m_family << ") addr.family(" << addr->getFamily()
        << ") not equal, addr=" << addr->toString();
        return false;
    }

    // 检查是否是unix address
    UnixAddress::Ptr uaddr = std::dynamic_pointer_cast<UnixAddress>(addr);
    if(uaddr) {
        Socket::Ptr sock = Socket::CreateUnixTCPSocket();
        if(sock->connect(uaddr)) {
            return false;
        } else {
            sylar::FSUtil::Unlink(uaddr->getPath(), true);
        }
    }

    if(::bind(m_sock, addr->getAddr(), addr->getAddrLen())) {
        Log_Error(g_logger) << "bind error errrno=" << errno
            << " errstr=" << strerror(errno);
        return false;
    }
    getLocalAddress();
    return true;
}

bool Socket::connect(const Address::Ptr address, uint64_t timeout_ms) {
    if(!isValid()) {
        newSock();
        if(!isValid()) {
            return false;
        }
    }

    if(UNLIKELY(address->getFamily() != m_family)) {
        Log_Error(g_logger) << "connect sock.family("
        << m_family << ") addr.family(" << address->getFamily()
        << ") not equal, addr=" << address->toString();
        return false;
    }

    m_remoteAddress = address;
    if(timeout_ms == -1) {
        if(::connect(m_sock, address->getAddr(), address->getAddrLen())) {
            Log_Error(g_logger) << "sock=" << m_sock << " connect(" << address->toString()
                << ") error errno=" << errno << " errstr=" << strerror(errno);
            close();
            return false;
        }
    }
    else {
        if(::connect_with_timeout(m_sock, address->getAddr(), address->getAddrLen(), timeout_ms)) {
            Log_Error(g_logger) << "sock=" << m_sock << " connect(" << address->toString()
                << ") error errno=" << errno << " errstr=" << strerror(errno);
            close();
            return false;
        }
    }
    m_Connected = true;
    getLocalAddress();
    getRemoteAddress();
    return true;
}

bool Socket::reconnect(uint64_t timeout_ms) {
    if(!m_remoteAddress) {
        Log_Error(g_logger) << "reconnect m_remoteAddress is null";
        return false;
    }
    m_localAddress.reset();
    return connect(m_remoteAddress, timeout_ms);
}

bool Socket::listen(int backlog) {
    if(!isValid()) {
        Log_Error(g_logger) << "listen error sock=-1";
        return false;
    }
    if(::listen(m_sock, backlog)) {
        Log_Error(g_logger) << "listen error errno=" << errno
            << " errstr=" << strerror(errno);
        return false;
    }
    return true;
}

bool Socket::close() {
    if(!m_Connected && m_sock == -1) {
        return true;
    }
    m_Connected = false;
    if(m_sock != -1) {
        ::close(m_sock);
        m_sock = -1;
    }
    return false; //?
}

int Socket::send(const void* buffer, size_t length, int flags) {
    if(isConnected()) {
        return ::send(m_sock, buffer, length, flags);
    }
    return -1;
}

int Socket::send(const iovec* buffers, size_t length, int flags) {
    if(isConnected()) {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffers;
        msg.msg_iovlen = length;
        return ::sendmsg(m_sock, &msg, flags);
    }
    return -1;
}

int Socket::sendTo(const void* buffer, size_t length, const Address::Ptr to, int flags) {
    if(isConnected()) {
        return ::sendto(m_sock, buffer, length, flags, to->getAddr(), to->getAddrLen());
    }
    return -1;
}

int Socket::sendTo(const iovec* buffers, size_t length, const Address::Ptr to, int flags) {
    if(isConnected()) {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffers;
        msg.msg_iovlen = length;
        msg.msg_name = to->getAddr();
        msg.msg_namelen = to->getAddrLen();
        return ::sendmsg(m_sock, &msg, flags);   
    }
    return -1;
}

int Socket::recv(void* buffer, size_t length, int flags) {
    if(isConnected()) {
        return ::recv(m_sock, buffer, length, flags);
    }
    return -1;
}

int Socket::recv(iovec* buffers, size_t length, int flags) {
    if(isConnected()) {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffers;
        msg.msg_iovlen = length;
        return ::recvmsg(m_sock, &msg, flags);
    }
    return -1;
}

int Socket::recvFrom(void* buffer, size_t length, const Address::Ptr from, int flags) {
    if(isConnected()) {
        socklen_t len = from->getAddrLen();
        return ::recvfrom(m_sock, buffer, length, flags, from->getAddr(), &len);
    }
    return -1;
}

int Socket::recvFrom(iovec* buffers, size_t length, const Address::Ptr from, int flags) {
    if(isConnected()) {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffers;
        msg.msg_iovlen = length;
        msg.msg_name = from->getAddr();
        msg.msg_namelen = from->getAddrLen();
        return ::recvmsg(m_sock, &msg, flags);
    }
    return -1;
}

Address::Ptr Socket::getLocalAddress() {
    if(m_localAddress) {
        return m_localAddress;
    }

    Address::Ptr result;
    switch(m_family) {
        case AF_INET:
            result.reset(new IPv4Address());
            break;
        case AF_INET6:
            result.reset(new IPv6Address());
            break;
        case AF_UNIX:
            result.reset(new UnixAddress());
            break;
        default:
            result.reset(new UnknownAddress(m_family));
            break;
    }

    auto addrlen = result->getAddrLen();
    if(getsockname(m_sock, result->getAddr(), &addrlen)) {
        Log_Error(g_logger) << "getsockname error sock=" << m_sock
            << " errno=" << errno << " errstr=" << strerror(errno);
        return Address::Ptr(new UnknownAddress(m_family));
    }
    if(m_family == AF_UNIX) {
        UnixAddress::Ptr addr = std::dynamic_pointer_cast<UnixAddress>(result);
        addr->setAddrLen(addrlen);
    }
    m_localAddress = result;
    return m_localAddress;
}

Address::Ptr Socket::getRemoteAddress() {
    if(m_remoteAddress) {
        return m_remoteAddress;
    }

    Address::Ptr result;
    switch(m_family) {
        case AF_INET:
            result.reset(new IPv4Address());
            break;
        case AF_INET6:
            result.reset(new IPv6Address());
            break;
        case AF_UNIX:
            result.reset(new UnixAddress());
            break;
        default:
            result.reset(new UnknownAddress(m_family));
            break;
    }
    
    auto addrlen = result->getAddrLen();
    if(getpeername(m_sock, result->getAddr(), &addrlen)) {
        Log_Error(g_logger) << "getpeername error sock=" << m_sock
           << " errno=" << errno << " errstr=" << strerror(errno);
        return Address::Ptr(new UnknownAddress(m_family));
    }
    if(m_family == AF_UNIX) {
        UnixAddress::Ptr addr = std::dynamic_pointer_cast<UnixAddress>(result);
        addr->setAddrLen(addrlen);
    }
    m_remoteAddress = result;
    return m_remoteAddress;
}

int Socket::getError() {
    int error = 0;
    socklen_t len = sizeof(error);
    if(!getOption(SOL_SOCKET, SO_ERROR, &error, &len)) {
        error = errno;
    }
    return error;
}

std::ostream& Socket::dump(std::ostream& os) const {
    os << "[Socket sock=" << m_sock
       << " is_connected=" << m_Connected
       << " family=" << m_family
       << " type=" << m_type
       << " protocol=" << m_protocol;
    if(m_localAddress) {
        os << " local_address=" << m_localAddress->toString();
    }
    if(m_remoteAddress) {
        os << " remote_address=" << m_remoteAddress->toString();
    }
    os << "]";
    return os;
}

std::string Socket::toString() const {
    std::stringstream ss;
    dump(ss);
    return ss.str();
}

bool Socket::cancelRead() {
    return EventPoller::getThis()->cancelEvent(m_sock, sylar::EventPoller::READ);
}

bool Socket::cancelWrite() {
    return EventPoller::getThis()->cancelEvent(m_sock, sylar::EventPoller::WRITE);
}

bool Socket::cancelAccept() {
    return EventPoller::getThis()->cancelEvent(m_sock, sylar::EventPoller::READ);
}

bool Socket::cancelAll() {
    return EventPoller::getThis()->cancelAll(m_sock);
}

} // namespace sylar
