#ifndef COURSE_SOCKET_COMPAT_H
#define COURSE_SOCKET_COMPAT_H

// For windows
#ifdef WIN32
#pragma comment(lib, "ws2_32.lib")
#define WIN32_LEAN_AND_MEAN
#undef TEXT
#include <winsock2.h>
#include <ws2tcpip.h>

// For linux
#else
#define sprintf_s sprintf
typedef int SOCKET;
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
static const int INVALID_SOCKET = -1;
static const int SOCKET_ERROR = -1;
#endif

#include <cstdio>
#include <stdexcept>

class Socket {
public:
    static void initSockets() {
#ifdef WIN32
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            char message[500];
            sprintf_s(message, "WSAStartup() failed with error: %d\n", iResult);
            throw std::runtime_error(message);
        }
#endif
    }

    static void cleanupSockets() {
#ifdef WIN32
        WSACleanup();
#endif
    }

protected:
    SOCKET _sock;
    char _message[200];

    Socket() : _sock(INVALID_SOCKET), _message("") {}
    explicit Socket(SOCKET sock) : _sock(sock), _message("") {}

    void inetPton(const char * host, struct sockaddr_in & saddr_in) {
#ifdef WIN32
        char wsz[64];
        sprintf_s(wsz, "%S", host);
        InetPton(AF_INET, host, &(saddr_in.sin_addr.s_addr));
#else
        inet_pton(AF_INET, host, &(saddr_in.sin_addr));
#endif
    }

public:

    void closeConnection() {
#ifdef WIN32
        closesocket(_sock);
#else
        close(_sock);
#endif
        _sock = INVALID_SOCKET;
    }

    const char* getMessage() {
        return _message;
    }
};


#endif //COURSE_SOCKET_COMPAT_H
