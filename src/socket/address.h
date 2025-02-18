#ifndef _SYLAR_ADDRESS_H_
#define _SYLAR_ADDRESS_H_

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>

namespace sylar {

class IPAddress;

class Address {
public:
    using Ptr = std::shared_ptr<Address>;

    static Address::Ptr Create(const sockaddr* addr, socklen_t addrlen);

    // 域名 -> 地址
    static bool Lookup(std::vector<Address::Ptr> &result, const std::string& host, 
                        int family = AF_INET, int type = 0, int protocol = 0);

    static Address::Ptr LookupAny(const std::string& host,
        int family = AF_INET, int type = 0, int protocol = 0);

    static std::shared_ptr<IPAddress> LookupAnyIPAddress(const std::string& host,
        int family = AF_INET, int type = 0, int protocol = 0);

    // 查找本地全部网卡
    static bool GetInterfaceAddresses(std::multimap<std::string ,std::pair<Address::Ptr, uint32_t> >& result, 
                                    int family = AF_INET);

    // 查找本地指定名称的网卡
    static bool GetInterfaceByName(std::vector<std::pair<Address::Ptr, uint32_t> >&result,
                                    const std::string& iface, int family = AF_INET);

    Address() = default;

    virtual ~Address() = default;

    int getFamily() const;

    virtual sockaddr* getAddr() = 0;

    virtual const sockaddr* getAddr() const = 0;

    virtual socklen_t getAddrLen() const = 0;

    virtual std::ostream& print(std::ostream& ofs) const = 0;

    std::string toString() const;

    bool operator<(const Address& rhs) const;

    bool operator==(const Address& rhs) const;

    bool operator!=(const Address& rhs) const;
};

class IPAddress : public Address {
public:
    using Ptr = std::shared_ptr<IPAddress>;

    static IPAddress::Ptr Create(const char* addr, uint16_t port = 0);

    virtual IPAddress::Ptr broadcastAddress(uint32_t prefix_len) = 0;

    virtual IPAddress::Ptr networkAddress(uint32_t prefix_len) = 0;

    virtual IPAddress::Ptr subnetMask(uint32_t prefix_len) = 0;

    virtual uint32_t getPort() const = 0;

    virtual void setPort(uint16_t v) = 0;
};

class IPv4Address : public IPAddress {
public:
    using Ptr = std::shared_ptr<IPv4Address>;

    static IPv4Address::Ptr Create(const char* addr, uint16_t port = 0);

    IPv4Address(const sockaddr_in& address);

    IPv4Address(uint32_t address = INADDR_ANY, uint16_t port = 0);

    sockaddr* getAddr() override;

    const sockaddr* getAddr() const override;

    socklen_t getAddrLen() const override;

    std::ostream& print(std::ostream& ofs) const override;
    
    IPAddress::Ptr broadcastAddress(uint32_t prefix_len) override;

    IPAddress::Ptr networkAddress(uint32_t prefix_len) override;

    IPAddress::Ptr subnetMask(uint32_t prefix_len) override;

    uint32_t getPort() const override;

    void setPort(uint16_t v) override;
private:
    sockaddr_in m_addr;
};

class IPv6Address : public IPAddress {
public:
    using Ptr = std::shared_ptr<IPv6Address>;

    static IPv6Address::Ptr Create(const char* addr, uint16_t port = 0);

    IPv6Address();

    IPv6Address(const sockaddr_in6& address);

    IPv6Address(const uint8_t address[16], uint16_t port = 0);

    sockaddr* getAddr() override;

    const sockaddr* getAddr() const override;

    socklen_t getAddrLen() const override;

    std::ostream& print(std::ostream& ofs) const override;

    IPAddress::Ptr broadcastAddress(uint32_t prefix_len) override;

    IPAddress::Ptr networkAddress(uint32_t prefix_len) override;

    IPAddress::Ptr subnetMask(uint32_t prefix_len) override;

    uint32_t getPort() const override;

    void setPort(uint16_t v) override;
private:
    sockaddr_in6 m_addr;
};

class UnixAddress : public Address {
public:
    using Ptr = std::shared_ptr<UnixAddress>;

    UnixAddress();

    UnixAddress(const std::string& path);

    const sockaddr* getAddr() const override;

    sockaddr* getAddr() override;

    socklen_t getAddrLen() const override;

    void setAddrLen(uint32_t v);

    std::string getPath() const;

    std::ostream& print(std::ostream& ofs) const override;

private:
    sockaddr_un m_addr;
    socklen_t m_length;
};

class UnknownAddress: public Address {
public:
    using Ptr = std::shared_ptr<UnknownAddress>;

    UnknownAddress(int family);

    UnknownAddress(const sockaddr& addr);

    const sockaddr* getAddr() const override;

    sockaddr* getAddr() override;

    socklen_t getAddrLen() const override;

    std::ostream& print(std::ostream& ofs) const override;

private:
    sockaddr m_addr;
};

} // namespace sylar


#endif //_SYLAR_ADDRESS_H_