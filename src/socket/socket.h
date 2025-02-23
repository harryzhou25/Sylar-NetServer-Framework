#ifndef _SYLAR_SOCKET_H_
#define _SYLAR_SOCKET_H_

#include <memory>
#include <sys/socket.h>

#include "util/util.h"
#include "socket/address.h"

namespace sylar {

class Socket : public std::enable_shared_from_this<Socket>, noncopyable {
public:
    using Ptr = std::shared_ptr<Socket>;
    enum Type {
        TCP = SOCK_STREAM,
        UDP = SOCK_DGRAM
    };
    enum Family {
        IPv4 = AF_INET,
        IPv6 = AF_INET6,
        UNIX = AF_UNIX,
    };

    static Socket::Ptr CreateTCP(Address::Ptr address);
    static Socket::Ptr CreateUDP(Address::Ptr address);

    static Socket::Ptr CreateTCPSocket();
    static Socket::Ptr CreateUDPSocket();

    static Socket::Ptr CreateTCPSocket6();
    static Socket::Ptr CreateUDPSocket6();

    static Socket::Ptr CreateUnixTCPSocket();
    static Socket::Ptr CreateUnixUDPSocket();

    Socket(int family, int type, int protocol = 0);

    ~Socket();

    int64_t getSendTimeout();

    void setSendTimeout(int64_t v);

    int64_t getRecvTimeout();

    void setRecvTimeout(int64_t v);

    bool getOption(int level, int option, void* result, socklen_t* len);

    template<class T>
    bool getOption(int level, int option, T& result) {
        socklen_t length = sizeof(T);
        return getOption(level, option, &result, &length);
    }

    bool setOption(int level, int option, const void* result, socklen_t len);

    template<class T>
    bool setOption(int level, int option, const T& value) {
        return setOption(level, option, &value, sizeof(T));
    }

    Socket::Ptr accept();

    bool bind(const Address::Ptr addr);

    bool connect(const Address::Ptr addr, uint64_t timeout_ms = -1);

    bool reconnect(uint64_t timeout_ms = -1);

    bool listen(int backlog = SOMAXCONN);

    bool close();

    int send(const void* buffer, size_t length, int flags = 0);
    int send(const iovec* buffers, size_t length, int flags = 0);

    int sendTo(const void* buffer, size_t length, const Address::Ptr to, int flags = 0);
    int sendTo(const iovec* buffers, size_t length, const Address::Ptr to, int flags = 0);

    int recv(void* buffer, size_t length, int flags = 0);
    int recv(iovec* buffers, size_t length, int flags = 0);

    int recvFrom(void* buffer, size_t length, const Address::Ptr from, int flags = 0);
    int recvFrom(iovec* buffers, size_t length, const Address::Ptr from, int flags = 0);

    Address::Ptr getLocalAddress();
    Address::Ptr getRemoteAddress();

    int getSock() const { return m_sock; }

    int getType() const { return m_type; }

    int getFamily() const { return m_family; }

    int getProtocol() const { return m_protocol; }

    bool isConnected() const { return m_Connected; }

    bool isValid() const { return (m_sock != -1); };

    int getError();

    std::ostream& dump(std::ostream& os) const;

    std::string toString() const;

    bool cancelRead();

    bool cancelWrite();

    bool cancelAccept();

    bool cancelAll();

protected:
    void initSock();

    void newSock();

    bool init(int sock);
private:
    int m_sock;
    int m_type;
    int m_family;
    int m_protocol;
    bool m_Connected;
    Address::Ptr m_localAddress;
    Address::Ptr m_remoteAddress;
};

} // namespace sylar

#endif //_SYLAR_SOCKET_H_