#ifndef COURSE_PROTO_BUFFER_H
#define COURSE_PROTO_BUFFER_H

#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>

class IProtoBufferListener {
public:
    virtual void protobuffer_updated() = 0;
};

class ProtoBuffer {
public:
    ProtoBuffer();
    ~ProtoBuffer();

    void set_listener(IProtoBufferListener* listener);
    void close();

    void append(const std::vector<char>& data);
    void append(const char* data, int sz);
    int recv(class TcpSocket&);
    int send(class TcpSocket&);

    void read(std::string& v);
    void read(int& v);
    void read(double& v);
    void read(std::vector<double>& v);
    void read(std::vector<int>& v);

    void write(const std::string& v);
    void write(int v);
    void write(double v);
    void write(const std::vector<double>& v);
    void write(const std::vector<int>& v);

    const std::vector<char>& data();

    std::mutex& mutex() { return mut; }
    bool is_closed() const { return closed; }

private:
    volatile bool closed = false;
    std::vector<char> buf;
    std::mutex mut;
    std::condition_variable cv;
    IProtoBufferListener* listener = nullptr;
    std::thread* conn_thread = nullptr;
    volatile bool buffer_updated = false;

    void notify_updates();
};





#endif //COURSE_PROTO_BUFFER_H
