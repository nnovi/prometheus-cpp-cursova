#ifndef COURSE_TCP_SERVER_SOCKET_H
#define COURSE_TCP_SERVER_SOCKET_H

#include "tcp_socket.h"

class TcpServerSocket : public TcpSocket {
public:
    TcpServerSocket(const char * host, int port) : TcpSocket(host, port) {
        const int enable = 1;
        setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));

        if (bind(_sock, _addressInfo->ai_addr, (int)_addressInfo->ai_addrlen) == SOCKET_ERROR) {
            sprintf_s(_message, "bind() failed");
            throw std::runtime_error(_message);
        }

        // Listen for a connection, exiting on failure
        if (listen(_sock, SOMAXCONN)  == -1) {
            sprintf_s(_message, "listen() failed");
            throw std::runtime_error(_message);
        }

        if (port == 0) {
            socklen_t sz = sizeof(*(_addressInfo->ai_addr));
            if (getsockname(_sock, _addressInfo->ai_addr, &sz) != 0) {
                sprintf_s(_message, "getsockname() failed");
                throw std::runtime_error(_message);
            }

            _port = ntohs(((sockaddr_in*)(_addressInfo->ai_addr))->sin_port);
        }
    }

    TcpSocket* acceptConnection() {
        // Accept connection, exiting on failure
        sockaddr_in sin;
        socklen_t sz = sizeof(sin);
        SOCKET conn = accept(_sock, (struct sockaddr *)&sin, &sz);
        if (conn == SOCKET_ERROR) {
            sprintf_s(_message, "accept() failed");
            return nullptr;
        }

        return new TcpSocket(conn, &sin);
    }
};

#endif //COURSE_TCP_SERVER_SOCKET_H
