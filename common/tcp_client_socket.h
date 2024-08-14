#ifndef COURSE_TCP_CLIENT_SOCKET_H
#define COURSE_TCP_CLIENT_SOCKET_H

#include "tcp_socket.h"

class TcpClientSocket : public TcpSocket {
public:
    TcpClientSocket(const std::string& host, int port) : TcpSocket(host, port) {}

    bool openConnection() {
        // Connect to server, returning on failure
        if (connect(_sock, _addressInfo->ai_addr, (int)_addressInfo->ai_addrlen) == SOCKET_ERROR) {
            sprintf_s(_message, "connect() failed; please make sure server is running");
            return false;
        }

        _connected = true;
        return true;
    }
};

#endif //COURSE_TCP_CLIENT_SOCKET_H
