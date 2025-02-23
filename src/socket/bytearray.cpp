#include "socket/bytearray.h"
#include "socket/endian.h"
#include "log/logger.h"

#include <fstream>
#include <sstream>
#include <string.h>
#include <iomanip>

namespace sylar {
static Logger::Ptr g_logger = Name_Logger("system");

ByteArray::Node::Node():
    size(0),
    data(nullptr),
    next(nullptr) {}

ByteArray::Node::Node(size_t s):
        size(s),
        data(new char[s]),
        next(nullptr) {}

ByteArray::Node::~Node() {
    if(data) {
        delete[] data;
    }
}

ByteArray::ByteArray(size_t base_size)
            :m_baseSize(base_size),
            m_position(0),
            m_capacity(base_size),
            m_size(0),
            m_endian(SYLAR_BIG_ENDIAN) {
    m_root = new Node(m_baseSize);
    m_cur = m_root;
}

ByteArray::~ByteArray() {
    Node* tmp = m_root;
    while(tmp) {
        m_cur = tmp;
        tmp = tmp->next;
        delete m_cur;
    }
}

static uint32_t EncodeZigzag32(const int32_t& v) {
    if(v < 0) {
        return ((uint32_t)(-v)) * 2 - 1;
    } else {
        return v * 2;
    }
}

static uint64_t EncodeZigzag64(const int64_t& v) {
    if(v < 0) {
        return ((uint64_t)(-v)) * 2 - 1;
    } else {
        return v * 2;
    }
}

static int32_t DecodeZigzag32(const uint32_t& v) {
    return (v >> 1) ^ -(v & 1);
}

static int64_t DecodeZigzag64(const uint64_t& v) {
    return (v >> 1) ^ -(v & 1);
}

void ByteArray::writeFint8(int8_t value) {
    write(&value, sizeof(value));
}

void ByteArray::writeFuint8(uint8_t value) {
    write(&value, sizeof(value));
}

void ByteArray::writeFint16(int16_t value) {
    if(m_endian != SYLAR_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFuint16(uint16_t value) {
    if(m_endian != SYLAR_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFint32(int32_t value) {
    if(m_endian != SYLAR_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}
void ByteArray::writeFuint32(uint32_t value) {
    if(m_endian != SYLAR_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFint64(int64_t value) {
    if(m_endian != SYLAR_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFuint64(uint64_t value) {
    if(m_endian != SYLAR_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeInt32(int32_t value) {
    writeUint32(EncodeZigzag32(value));
}
void ByteArray::writeUint32(uint32_t value) {
    uint8_t data[5];
    uint8_t i = 0;
    while(value >= 0x80) {
        data[i++] = (value & 0x7f) | 0x80;
        value >>= 7;
    }
    write(data, i);
}

void ByteArray::writeInt64(int64_t value) {
    writeUint32(EncodeZigzag64(value));
}
void ByteArray::writeUint64(uint64_t value) {
    uint8_t data[10];
    uint8_t i = 0;
    while(value >= 0x80) {
        data[i++] = (value & 0x7f) | 0x80;
        value >>= 7;
    }
    write(data, i);
}

void ByteArray::writeFloat(float value) {
    uint32_t v;
    memcpy(&v, &value, sizeof(value));
    writeFuint32(v);
}

void ByteArray::writeDouble(double value) {
    uint64_t v;
    memcpy(&v, &value, sizeof(value));
    writeFuint64(v);
}

void ByteArray::writeStringF16(const std::string& value) {
    writeFuint16(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringF32(const std::string& value) {
    writeFuint32(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringF64(const std::string& value) {
    writeFuint64(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringVint(const std::string& value) {
    writeUint64(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeString(const std::string& value) {
    write(value.c_str(), value.size());
}

int8_t ByteArray::readFint8() {
    int8_t v;
    read(&v, sizeof(v));
    return v;
}

uint8_t ByteArray::readFuint8() {
    uint8_t v;
    read(&v, sizeof(v));
    return v;
}

#define readTemplate(type) \
    type v; \
    read(&v, sizeof(v)); \
    if(m_endian == SYLAR_BYTE_ORDER) return v; \
    else return byteswap(v);

    int16_t ByteArray::readFint16() {
        readTemplate(int16_t);
    }

    uint16_t ByteArray::readFuint16() {
        readTemplate(uint16_t);
    }

    int32_t ByteArray::readFint32() {
        readTemplate(int32_t);
    }

    uint32_t ByteArray::readFuint32() {
        readTemplate(uint32_t);
    }

    int64_t ByteArray::readFint64() {
        readTemplate(int64_t);
    }
    
    uint64_t ByteArray::readFuint64() {
        readTemplate(uint64_t);
    }
#undef readTemplate

int32_t ByteArray::readInt32() {
    return DecodeZigzag32(readUint32());
}
int32_t ByteArray::readUint32() {
    uint32_t res = 0;
    for(int i = 0; i < 32; i += 7) {
        uint8_t t = readFuint8();
        if(t < 0x80) {
            res |= (uint32_t(t)) << i;
            break;
        }
        else {
            res |= (uint32_t(t) & 0x7f) << i;
        }
    }
    return res;
}

int32_t ByteArray::readInt64() {
    return DecodeZigzag64(readUint64());
}
int32_t ByteArray::readUint64() {
    uint64_t res = 0;
    for(int i = 0; i < 64; i += 7) {
        uint8_t t = readFuint8();
        if(t < 0x80) {
            res |= (uint64_t(t)) << i;
            break;
        }
        else {
            res |= (uint64_t(t) & 0x7f) << i;
        }
    }
    return res;
}

float ByteArray::readFloat() {
    uint32_t v = readFuint32();
    float value;
    memcpy(&value, &v, sizeof(v));
    return value;
}

double ByteArray::readDouble() {
    uint64_t v = readFuint64();
    float value;
    memcpy(&value, &v, sizeof(v));
    return value;
}

std::string ByteArray::readStringF16() {
    uint16_t len = readFuint16();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

std::string ByteArray::readStringF32() {
    uint32_t len = readFuint32();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

std::string ByteArray::readStringF64() {
    uint64_t len = readFuint64();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

std::string ByteArray::readStringVint() {
    uint64_t len = readUint64();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

void ByteArray::clear() {
    m_position = m_size = 0;
    m_capacity = m_baseSize;
    Node* tmp = m_root->next;
    while(tmp) {
        m_cur = tmp;
        tmp = tmp->next;
        delete m_cur;
    }
    m_cur = m_root;
    m_root->next = NULL;
}

void ByteArray::write(const void* buf, size_t size) {
    if(size <= 0) {
        return;
    }

    addCapacity(size);

    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    size_t bpos = 0;

    while(size > 0) {
        if(ncap >= size) {
            memcpy(m_cur->data + npos, (const char*)buf + bpos, size);
            ncap -= size;
            if(ncap == 0) {
                m_cur = m_cur->next;
            }
            m_position += size;
            bpos += size;
            size = 0;
        }
        else {
            memcpy(m_cur->data + npos, (const char*)buf + bpos, ncap);
            m_position += ncap;
            bpos += ncap;
            size -= ncap;
            m_cur = m_cur->next;
            ncap = m_cur->size;
            npos = 0;
        }
    }
    
    if(m_position > m_size) {
        m_size = m_position;
    }
}

void ByteArray::read(void* buf, size_t size) {
    if(size > getReadSize()) {
        throw std::out_of_range("ByteArray::read(void* buf, size_t size): input size too large");
    }

    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    size_t bpos = 0;

    while(size > 0) {
        if(ncap >= size) {
            memcpy((char*)buf + bpos, m_cur->data + npos, size);
            if(size == ncap) {
                m_cur = m_cur->next;
            }
            m_position += size;
            bpos += size;
            size = 0;
        }
        else {
            memcpy((char*)buf + bpos, m_cur->data + npos, ncap);
            m_cur = m_cur->next;
            npos = 0;
            bpos += ncap;
            size -= ncap;
            m_position += ncap;
            ncap = m_cur->size;
        }
    }
}

void ByteArray::read(void* buf, size_t size, size_t st) const {
    if(size > m_size - st) {
        throw std::out_of_range("ByteArray::read(void* buf, size_t size, size_t st): input size too large");
    }

    size_t npos = st % m_baseSize;
    size_t ncap = m_cur->size - npos;
    size_t bpos = 0;
    auto cur = m_cur;

    while(size > 0) {
        if(ncap >= size) {
            memcpy((char*)buf + bpos, cur->data + npos, size);
            if(size == ncap) {
                cur = cur->next;
            }
            st += size;
            bpos += size;
            size = 0;
        }
        else {
            memcpy((char*)buf + bpos, cur->data + npos, ncap);
            cur = cur->next;
            npos = 0;
            bpos += ncap;
            size -= ncap;
            st += ncap;
            ncap = cur->size;
        }
    }
}

void ByteArray::setPosition(size_t v) {
    if(v > m_capacity) {
        throw std::out_of_range("ByteArray::set_position(size_t v): input size out of bound");
    }
    m_position = v;
    if(m_position > m_size) {
        m_size = m_position;
    }
    m_cur = m_root;
    while(v >= m_cur->size) {
        v -= m_cur->size;
        m_cur = m_cur->next;
    }
}

bool ByteArray::writeToFile(const std::string& name) const {
    std::ofstream ofs;
    ofs.open(name, std::ios::trunc | std::ios::binary);
    if(!ofs) {
        return false;
    }

    int64_t read_size = getReadSize();
    int64_t pos = m_position;
    Node* cur = m_cur;

    while(read_size > 0) {
        int diff = pos % m_baseSize;
        int64_t len = (read_size > (int64_t)m_baseSize ? m_baseSize : read_size) - diff;
        ofs.write(cur->data + diff, len);
        cur = cur->next;
        pos += len;
        read_size -= len;
    }

    return true;
}

bool ByteArray::readFromFile(const std::string& name) {
    std::ifstream ifs;
    ifs.open(name, std::ios::binary);
    if(!ifs) {
        return false;
    }

    std::shared_ptr<char> buff(new char[m_baseSize], [](char* ptr) { delete[] ptr;});
    while(!ifs.eof()) {
        ifs.read(buff.get(), m_baseSize);
        write(buff.get(), ifs.gcount());
    }
    return true;
}

void ByteArray::addCapacity(size_t size) {
    if(size == 0) {
        return;
    }
    auto _cap = getCapacity();
    if(_cap >= size) {
        return;
    }

    size -= _cap;
    size_t num = ceil(1.0* size / m_baseSize);
    auto tail = m_root;
    while(tail->next) {
        tail = tail->next;
    }

    Node* first = NULL;
    for(size_t i = 0; i < num; ++i) {
        tail->next = new Node(m_baseSize);
        if(first == NULL) {
            first = tail->next;
        }
        tail = tail->next;
        m_capacity += m_baseSize;
    }

    //只有root的情况
    if(_cap == 0) {
        m_cur = first;
    }
}

std::string ByteArray::toString() const {
    std::string str;
    str.resize(getReadSize());
    if(str.empty()) {
        return str;
    }
    read(&str[0], str.size(), m_position);
    return str;
}

std::string ByteArray::toHexString() const {
    std::string str = toString();
    std::stringstream ss;

    for(size_t i = 0; i < str.size(); ++i) {
        if(i > 0 && i % 32 == 0) {
            ss << std::endl;
        }
        ss << std::setw(2) << std::setfill('0') << std::hex
           << (int)(uint8_t)str[i] << " ";
    }

    return ss.str();
}

uint64_t ByteArray::getReadBuffers(std::vector<iovec>& buffers, uint64_t len) const {
    len = len > getReadSize() ? getReadSize() : len;
    if(len == 0) {
        return 0;
    }

    uint64_t size = len;

    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    struct iovec iov;
    Node* cur = m_cur;

    while(len > 0) {
        if(ncap >= len) {
            iov.iov_base = cur->data + npos;
            iov.iov_len = len;
            len = 0;
        } else {
            iov.iov_base = cur->data + npos;
            iov.iov_len = ncap;
            len -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
        buffers.push_back(iov);
    }
    return size;
}

uint64_t ByteArray::getReadBuffers(std::vector<iovec>& buffers
                                ,uint64_t len, uint64_t position) const {
    len = len > getReadSize() ? getReadSize() : len;
    if(len == 0) {
        return 0;
    }

    uint64_t size = len;

    size_t npos = position % m_baseSize;
    size_t count = position / m_baseSize;
    Node* cur = m_root;
    while(count > 0) {
        cur = cur->next;
        --count;
    }

    size_t ncap = cur->size - npos;
    struct iovec iov;
    while(len > 0) {
        if(ncap >= len) {
            iov.iov_base = cur->data + npos;
            iov.iov_len = len;
            len = 0;
        } else {
            iov.iov_base = cur->data + npos;
            iov.iov_len = ncap;
            len -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
        buffers.push_back(iov);
    }
    return size;
}

uint64_t ByteArray::getWriteBuffers(std::vector<iovec>& buffers, uint64_t len) {
    if(len == 0) {
        return 0;
    }
    addCapacity(len);
    uint64_t size = len;

    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    struct iovec iov;
    Node* cur = m_cur;
    while(len > 0) {
        if(ncap >= len) {
            iov.iov_base = cur->data + npos;
            iov.iov_len = len;
            len = 0;
        } else {
            iov.iov_base = cur->data + npos;
            iov.iov_len = ncap;
            len -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
        buffers.push_back(iov);
    }
    return size;
}

bool ByteArray::isLittleEndian() const {return m_endian == SYLAR_LITTLE_ENDIAN;}

void ByteArray::setEndian(bool flag) {
    if(flag) {
        m_endian = SYLAR_LITTLE_ENDIAN;
    } 
    else {
        m_endian = SYLAR_BIG_ENDIAN;
    }
}

} // namespace sylar
