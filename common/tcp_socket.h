#ifndef COURSE_TCP_SOCKET_H
#define COURSE_TCP_SOCKET_H

#include <stdexcept>
#include <string>
#include "socket_compat.h"

class TcpSocket : public Socket {
protected:
    std::string _host;
    int _port;
    bool _connected;
    addrinfo* _addressInfo;

public:
    TcpSocket(const std::string& host, int port) : _host(host) {
        std::string port_s = std::to_string(port);
        _port = port;

        // No connection yet
        _sock = INVALID_SOCKET;
        _connected = false;
        *_message = 0;

        // Set up client address info
        struct addrinfo hints = {0};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        // Resolve the server address and port, returning on failure
        _addressInfo = nullptr;
        int iResult = getaddrinfo(_host.c_str(), port_s.c_str(), &hints, &_addressInfo);
        if ( iResult != 0 ) {
            sprintf_s(_message, "getaddrinfo() failed with error: %d", iResult);
            throw std::runtime_error(_message);
        }

        // Create a SOCKET
        _sock = socket(_addressInfo->ai_family, _addressInfo->ai_socktype, _addressInfo->ai_protocol);
        if (_sock == INVALID_SOCKET) {
            sprintf_s(_message, "socket() failed");
            throw std::runtime_error(_message);
        }
    }

    // wrap existing connected socket
    TcpSocket(SOCKET sock, sockaddr_in* sin) : Socket(sock), _connected(true), _addressInfo(nullptr) {
        _host = inet_ntoa(sin->sin_addr);
        _port = ntohs(sin->sin_port);
    }

    int sendData(const char* buf, int len) const {
        return send(_sock, buf, len, 0);
    }

    int receiveData(char* buf, int len) const {
        return recv(_sock, (char *)buf, len, 0);
    }

    bool isConnected() const {
        return _connected;
    }

    const std::string& getHost() const {
        return _host;
    }

    int getPort() const {
        return _port;
    }

    SOCKET getSocket() const {
        return _sock;
    }
};

#endif //COURSE_TCP_SOCKET_H
