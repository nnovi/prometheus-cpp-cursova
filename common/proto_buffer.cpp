#include <stdexcept>
#include <iostream>
#include "proto_buffer.h"
#include "tcp_socket.h"

#define MAX_BUFFER 256*1024

ProtoBuffer::ProtoBuffer() = default;
ProtoBuffer::~ProtoBuffer() {
    close();
    if (conn_thread != nullptr) {
        conn_thread->join();
        delete conn_thread;
        conn_thread = nullptr;
    }
};

void ProtoBuffer::set_listener(IProtoBufferListener* listener) {
    this->listener = listener;
}

void ProtoBuffer::close() {
    if (closed) return;
    std::lock_guard<std::mutex> lock(mut);

    closed = true;
    notify_updates();
}

void ProtoBuffer::append(const std::vector<char> &data) {
    if (data.empty()) return;

    std::lock_guard<std::mutex> lock(mut);
    if (closed) {
        throw std::logic_error("attempt to append to closed buffer");
    }

    if (buf.size() + data.size() > MAX_BUFFER) {
        throw std::overflow_error("buffer overflow: max size is " + std::to_string(MAX_BUFFER));
    }

    buf.insert(buf.end(), data.begin(), data.end());
    notify_updates();
}

void ProtoBuffer::append(const char* data, int sz) {
    if (sz <= 0) return;

    std::lock_guard<std::mutex> lock(mut);
    if (closed) {
        throw std::logic_error("attempt to append to closed buffer");
    }

    if (buf.size() + sz > MAX_BUFFER) {
        throw std::overflow_error("buffer overflow: max size is " + std::to_string(MAX_BUFFER));
    }

    buf.insert(buf.end(), data, data+sz);
    notify_updates();
}

union proto_data {
    int iv;
    double dv;
    char raw[sizeof(double)];
};

void ProtoBuffer::read(int &v) {
    std::unique_lock<std::mutex> lock(mut);

    while (true) {
        if (closed) {
            throw std::logic_error("read from closed proto buffer");
        }
        if (buf.size() < sizeof(int)) {
            cv.wait(lock);
            continue;
        }
        proto_data* pd = (proto_data*)buf.data();
        v = pd->iv;
        buf.erase(buf.begin(), buf.begin()+sizeof(int));
        break;
    }
}

void ProtoBuffer::read(double &v) {
    std::unique_lock<std::mutex> lock(mut);

    while (true) {
        if (closed) {
            throw std::logic_error("read from closed proto buffer");
        }
        if (buf.size() < sizeof(double)) {
            cv.wait(lock);
            continue;
        }
        proto_data* pd = (proto_data*)buf.data();
        v = pd->dv;
        buf.erase(buf.begin(), buf.begin()+sizeof(double));
        break;
    }
}

void ProtoBuffer::read(std::string& v) {
    std::unique_lock<std::mutex> lock(mut);

    v.clear();
    while (true) {
        if (closed) {
            throw std::logic_error("read from closed proto buffer");
        }
        if (buf.size() < sizeof(int)) {
            cv.wait(lock);
            continue;
        }
        proto_data* pd = (proto_data*)buf.data();
        int sz = pd->iv;
        if (buf.size() < sizeof(int) + sz) {
            cv.wait(lock);
            continue;
        }
        v += std::string(buf.data() + sizeof(int), sz);
        buf.erase(buf.begin(), buf.begin()+sizeof(int)+sz);
        break;
    }
}

void ProtoBuffer::read(std::vector<int>& v) {
    std::unique_lock<std::mutex> lock(mut);

    v.clear();
    while (true) {
        if (closed) {
            throw std::logic_error("read from closed proto buffer");
        }
        if (buf.size() < sizeof(int)) {
            cv.wait(lock);
            continue;
        }
        proto_data* pd = (proto_data*)buf.data();
        int sz = pd->iv;
        if (buf.size() < sizeof(int) + sz*sizeof(int)) {
            cv.wait(lock);
            continue;
        }
        v.reserve(sz);
        for (int i=0; i<sz; i++) {
            pd = (proto_data*)(buf.data() + sizeof(int) + i*sizeof(int));
            v.push_back(pd->iv);
        }
        buf.erase(buf.begin(), buf.begin()+sizeof(int)+sz*sizeof(int));
        break;
    }
}

void ProtoBuffer::read(std::vector<double>& v) {
    std::unique_lock<std::mutex> lock(mut);

    v.clear();
    while (true) {
        if (closed) {
            throw std::logic_error("read from closed proto buffer");
        }
        if (buf.size() < sizeof(int)) {
            cv.wait(lock);
            continue;
        }
        proto_data* pd = (proto_data*)buf.data();
        int sz = pd->iv;
        if (buf.size() < sizeof(int) + sz*sizeof(double)) {
            cv.wait(lock);
            continue;
        }
        v.reserve(sz);
        for (int i=0; i<sz; i++) {
            pd = (proto_data*)(buf.data() + sizeof(int) + i*sizeof(double));
            v.push_back(pd->dv);
        }
        buf.erase(buf.begin(), buf.begin()+sizeof(int)+sz*sizeof(double));
        break;
    }
}

void ProtoBuffer::write(int v) {
    std::lock_guard<std::mutex> lock(mut);
    proto_data pv{.iv=v};
    buf.reserve(sizeof(int));
    for (int i=0; i<sizeof(int); i++) {
        buf.push_back(pv.raw[i]);
    }

    notify_updates();
}

void ProtoBuffer::write(double v) {
    std::lock_guard<std::mutex> lock(mut);
    proto_data pv{.dv=v};
    buf.reserve(sizeof(double));
    for (int i=0; i<sizeof(double); i++) {
        buf.push_back(pv.raw[i]);
    }

    notify_updates();
}

void ProtoBuffer::write(const std::string& v) {
    std::lock_guard<std::mutex> lock(mut);
    proto_data pv{.iv=static_cast<int>(v.size())};
    buf.reserve(sizeof(int) + v.size());
    for (int i=0; i<sizeof(int); i++) {
        buf.push_back(pv.raw[i]);
    }
    buf.insert(buf.end(), v.begin(), v.end());

    notify_updates();
}

void ProtoBuffer::write(const std::vector<double>& v) {
    std::lock_guard<std::mutex> lock(mut);
    proto_data pv{.iv=static_cast<int>(v.size())};
    buf.reserve(sizeof(int) + v.size()*sizeof(double));
    for (int i=0; i<sizeof(int); i++) {
        buf.push_back(pv.raw[i]);
    }
    for (double vi : v) {
        pv.dv = vi;
        for (int i=0; i<sizeof(double); i++) {
            buf.push_back(pv.raw[i]);
        }
    }

    notify_updates();
}

void ProtoBuffer::write(const std::vector<int>& v) {
    std::lock_guard<std::mutex> lock(mut);
    proto_data pv{.iv=static_cast<int>(v.size())};
    buf.reserve(sizeof(int) + v.size()*sizeof(int));
    for (int i=0; i<sizeof(int); i++) {
        buf.push_back(pv.raw[i]);
    }
    for (int vi : v) {
        pv.iv = vi;
        for (int i=0; i<sizeof(int); i++) {
            buf.push_back(pv.raw[i]);
        }
    }

    notify_updates();
}

void ProtoBuffer::notify_updates() {
    cv.notify_all();
    if (listener != nullptr) {
        listener->protobuffer_updated();
    }
    buffer_updated = true;
}

const std::vector<char>& ProtoBuffer::data() {
    return buf;
}

int ProtoBuffer::recv(TcpSocket& sock) {
    char buf[4096];
    int sz = sock.receiveData(buf, sizeof(buf));
    if (sz == 0) return sz;
    append(buf, sz);
    return sz;
}

int ProtoBuffer::send(TcpSocket& sock) {
    std::lock_guard<std::mutex> lock(mut);
    if (buf.empty()) return 0;
    int sz = sock.sendData(buf.data(), buf.size());
    if (sz == 0) return 0;

    if (buf.size() == sz) {
        buf.clear();
    } else {
        buf.erase(buf.begin(), buf.begin() + sz);
    }
    notify_updates();
    return sz;
}

