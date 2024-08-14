#include <iostream>
#include <thread>
#include <algorithm>
#include "../common/proto_buffer.h"
#include "../common/tcp_client_socket.h"
#include "../common/utils.h"
#ifndef WIN32
#include <poll.h>
#endif

struct Conn {
    ProtoBuffer in;
    ProtoBuffer out;
    TcpClientSocket& sock;
    std::thread* conn_thread = nullptr;

    void start() {
        conn_thread = new std::thread(&Conn::handle_connection, this);
    }

    void stop() {
        close();
        conn_thread->join();
    }

    void close() {
        in.close();
        out.close();
        sock.closeConnection();
    }


    void handle_connection() {
#ifdef WIN32
        WSAPOLLFD fds[1];
#else
        pollfd fds[1];
#endif
        fds[0].fd = sock.getSocket();
        {
            std::lock_guard<std::mutex> lock(out.mutex());
            if (out.data().empty()) {
                fds[0].events = POLLRDNORM;
            } else {
                fds[0].events = POLLRDNORM | POLLWRNORM;
            }
        }

        while (!out.is_closed() && sock.isConnected()) {
            {
                std::lock_guard<std::mutex> lock(out.mutex());
                if (out.data().empty()) {
                    fds[0].events = POLLRDNORM | POLLRDBAND;
                } else {
                    fds[0].events = POLLRDNORM | POLLWRNORM;
                }
            }
#ifdef WIN32
            int fd_events = WSAPoll(fds, 1, 1);
#else
            int fd_events = poll(fds, 1, 1);
#endif
            if (fd_events == 0) continue;

            if (fds[0].revents & (POLLHUP|POLLERR)) {
                break;
            }
            if (fds[0].revents & POLLRDNORM) {
                int sz = in.recv(sock);
                if (sz == 0) {
                    break;
                }
            }
            if (fds[0].revents & POLLWRNORM) {
                out.send(sock);
            }
            fds[0].revents = 0;
        }
        close();
    }

    void input(std::vector<double>& arr) {
        while (true) {
            std::string s;
            std::getline(std::cin, s);
            if (s.empty() || s == "X" || s == "x") break;

            try {
                arr.push_back(std::stod(s));
            } catch (...) {}
        }
    }

    void task1() {
        std::string name, result;
        std::cout << "Enter your name: ";
        std::getline(std::cin, name);
        out.write(name);
        in.read(result);

        std::cout << "Response: " << result << std::endl;
    }

    void task2() {
        double a, b, result;
        std::cout << "A=";
        std::cin >> a;
        std::cout << "B=";
        std::cin >> b;
        skip_spaces(std::cin);

        out.write(a);
        out.write(b);
        in.read(result);
        std::cout << "Result: " << result << std::endl;
    }

    void task3() {
        double a, b, result;
        in.read(a);
        in.read(b);
        result = a * b;
        std::cout << "Received: A=" << a << ", B=" << b << ". Sent result: " << result << std::endl;
        out.write(result);
    }

    void task4() {
        std::vector<double> arr;
        double result;
        std::cout << "Input array of numbers (use empty line or 'X' to stop input): ";
        input(arr);
        out.write(arr);
        in.read(result);
        std::cout << "Result: " << result << std::endl;
    }

    void task5() {
        double x;
        std::cout << "Enter number: ";
        std::cin >> x;
        skip_spaces(std::cin);

        out.write(x);
        in.read(x);
        std::cout << "Result: " << x << std::endl;
    }

    void task6() {
        int x;
        std::string result;
        std::cout << "Enter number: ";
        std::cin >> x;
        skip_spaces(std::cin);

        out.write(x);
        in.read(result);
        std::cout << "Result: " << result << std::endl;
    }

    void task7() {
        std::vector<double> x;
        in.read(x);
        std::cout << "received array to sort: " << x << std::endl;
        std::cout << std::endl;
        std::sort(x.begin(), x.end());
        std::cout << "returned sorted array: " << x << std::endl;
        std::cout << std::endl;
        out.write(x);
    }

    void task8() {
        std::string s;
        int result;

        std::cout << "Enter string: ";
        std::getline(std::cin, s);
        out.write(s);
        in.read(result);
        std::cout << "Result: " << result << std::endl;
    }

    void task9() {
        std::string s;
        in.read(s);
        std::string result(s.size(), '0');
        out.write(result);
        std::cout << "Got string: `" << s << "`. Sent as result: `" << result << "`" << std::endl;
    }

    void task10() {
        std::string s;
        std::getline(std::cin, s);
        out.write(s);
        in.read(s);
        std::cout << "Result: " << s << std::endl;
    }

    void task11() {
        std::string s;
        std::cout << "Enter your surname: ";
        std::getline(std::cin, s);
        out.write(s);
        in.read(s);
        std::cout << "Result: " << s << std::endl;
    }

    void task12() {
        double a, b, c, result;
        std::cout << "A = "; std::cin >> a;
        std::cout << "B = "; std::cin >> b;
        std::cout << "C = "; std::cin >> c;
        skip_spaces(std::cin);
        out.write(a);
        out.write(b);
        out.write(c);
        in.read(result);
        std::cout << "Result: " << result << std::endl;
    }

    void task13() {
        double a, b, c, result;
        in.read(a);
        in.read(b);
        in.read(c);
        std::cout << "Task: a=" << a << ", b=" << b << ", c=" << c << std::endl;
        result = 1;
        if (a < 0) result *= a;
        if (b < 0) result *= b;
        if (c < 0) result *= c;
        out.write(result);
        std::cout << "Sent result: " << result << std::endl;
    }

    void task14() {
        std::vector<double> arr;
        std::cout << "Input array of numbers (use empty line or 'X' to stop input): ";
        input(arr);
        out.write(arr);

        double min, max;
        in.read(min); in.read(max);
        std::cout << "Result: min=" << min << ", max=" << max << std::endl;
    }

    void task15() {
        double a,b,c,d;
        std::cout << "A = "; std::cin >> a;
        std::cout << "B = "; std::cin >> b;
        std::cout << "C = "; std::cin >> c;
        std::cout << "D = "; std::cin >> d;
        skip_spaces(std::cin);
        out.write(a);
        out.write(b);
        out.write(c);
        out.write(d);
        in.read(a);
        in.read(b);
        in.read(c);
        in.read(d);
        std::cout << "Result: " << a << ", " << b << ", " << c << ", " << d << std::endl;
    }

    void task16() {
        int x;
        std::cout << "Enter number: "; std::cin >> x;
        skip_spaces(std::cin);
        out.write(x);
        std::string result;
        in.read(result);
        std::cout << "Result: " << result << std::endl;
    }

    void task17() {
        std::vector<double> x;
        in.read(x);
        std::cout << "received array to sort: " << x << std::endl;
        std::sort(x.begin(), x.end());
        std::reverse(x.begin(), x.end());
        std::cout << "returned sorted array: " << x << std::endl;
        std::cout << std::endl;
        out.write(x);
    }

    void task18() {
        std::string s;
        std::cout << "Enter string: ";
        std::getline(std::cin, s);
        out.write(s);
        
        int result;
        in.read(result);
        std::cout << "Result: " << result << std::endl;
    }

    void task19() {
        std::string s;
        in.read(s);
        std::string result(s.size(), 'X');
        out.write(result);
        std::cout << "Got string: `" << s << "`. Sent as result: `" << result << "`" << std::endl;
    }  
    
    void task20() {
        std::string s;
        std::cout << "Enter string: ";
        std::getline(std::cin, s);
        out.write(s);

        std::string result;
        in.read(result);
        std::cout << "Result: " << result << std::endl;
    }

    void task21() {
        int num;
        std::cout << "Enter number: ";
        std::cin >> num;
        skip_spaces(std::cin);
        out.write(num);

        std::vector<int> result;
        in.read(result);
        std::cout << "Result: " << result << std::endl;
    }

    void task22() {
        int num;
        std::cout << "Enter number: ";
        std::cin >> num;
        skip_spaces(std::cin);
        out.write(num);

        std::string result;
        in.read(result);
        std::cout << "Result: " << result << std::endl;
    }

    void task23() {
        std::vector<double> arr;
        in.read(arr);

        std::cout << "Got array: " << arr << std::endl;
        for (auto& it : arr) {
            if (it < 0) it = 0;
        }

        out.write(arr);
        std::cout << "Sent modified array: " << arr << std::endl;
    }

    void task24() {
        std::string s;
        std::cout << "Enter string: ";
        std::getline(std::cin, s);
        out.write(s);

        int result;
        in.read(result);
        std::cout << "Result: " << result << std::endl;
    }

    void task25() {
        std::string s;
        in.read(s);
        std::cout << "Got string: " << s << std::endl;

        std::vector<int> result;
        result.reserve(s.size());
        for (int i=0; i<s.size(); i++) {
            result.push_back((rand() % 10000) - 5000);
        }
        out.write(result);
        std::cout << "Sent generated array: " << result << std::endl;
    }

    void task26() {
        std::string s;
        std::cout << "Enter string: ";
        std::getline(std::cin, s);
        out.write(s);

        std::string result;
        in.read(result);
        std::cout << "Result: " << result << std::endl;
    }

    void task27() {
        std::string s;
        std::cout << "Enter string: ";
        std::getline(std::cin, s);
        out.write(s);

        std::string result;
        in.read(result);
        std::cout << "Result: " << result << std::endl;
    }

    void task28() {
        double a,b,c,result;
        std::cout << "A=";
        std::cin >> a;
        std::cout << "B=";
        std::cin >> b;
        std::cout << "C=";
        std::cin >> c;
        skip_spaces(std::cin);
        out.write(a);
        out.write(b);
        out.write(c);
        in.read(result);
        std::cout << "Result: " << result << std::endl;
    }

    void task29() {
        std::string s1, s2, result;
        in.read(s1);
        std::cout << "Got string 1: " << s1 << std::endl;
        in.read(s2);
        std::cout << "Got string 2: " << s2 << std::endl;

        if (s1 == s2) {
            result = "Success";
        } else {
            result = "Failed";
        }
        out.write(result);
        std::cout << "Sent result: " << result << std::endl;
    }

    void task30() {
        std::vector<double> arr;
        double result;
        std::cout << "Input array of numbers (use empty line or 'X' to stop input): ";
        input(arr);
        out.write(arr);
        in.read(result);
        std::cout << "Result: " << result << std::endl;
    }

    void task31() {
        const int min_pings = 10;
        const long min_time = 1000;

        long total_time = 0;
        int pings = 0;

        int x;
        while (pings < min_pings || total_time < min_time) {
            auto start = std::chrono::high_resolution_clock::now();
            out.write(pings);
            in.read(x);
            auto finish = std::chrono::high_resolution_clock::now();

            total_time += std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count();
            ++pings;
        }
        out.write(-1);

        std::cout << "Ping: " << pings << " messages took " << total_time << "ms. Average latency: " << (double)total_time / (2*pings) << "ms." << std::endl;
    }
};

void print_usage(const std::string& cmd, const std::string& host, int port) {
    std::cout << "Usage: " << std::endl
              << cmd << " ||| Using default host/port: " << host << ":" << port << std::endl
              << cmd << " {PORT} ||| Using default host " << host << " with custom port" << std::endl
              << cmd << " {HOST} {PORT} ||| Using custom host and port" << std::endl;
}

int main(int argc, const char* argv[]) {
    std::string host = "127.0.0.1"; // default to localhost
    int port = 25000;               // default to 25000 port

    if (argc > 3) {
        print_usage(argv[0], host, port);
        exit(1);
    } else if (argc == 2) {
        port = atoi(argv[1]);
    } else if (argc == 3) {
        port = atoi(argv[2]);
        host = argv[1];
    }

    TcpSocket::initSockets();
    TcpClientSocket sock(host, port);

    if (!sock.openConnection()) {
        std::cout << sock.getMessage() << std::endl;
        exit(1);
    }

    Conn conn{
        .sock = sock,
    };
    conn.start();

    try {
        while (true) {
            int variant;
            std::cout << "Enter variant (1-31, 0 - exit): ";
            std::cin >> variant;
            skip_spaces(std::cin);
            if (variant <= 0 || variant > 31) {
                break;
            }
            conn.out.write(variant);   // task variant

            switch (variant) {
                case 1: conn.task1(); break;
                case 2: conn.task2(); break;
                case 3: conn.task3(); break;
                case 4: conn.task4(); break;
                case 5: conn.task5(); break;
                case 6: conn.task6(); break;
                case 7: conn.task7(); break;
                case 8: conn.task8(); break;
                case 9: conn.task9(); break;
                case 10: conn.task10(); break;
                case 11: conn.task11(); break;
                case 12: conn.task12(); break;
                case 13: conn.task13(); break;
                case 14: conn.task14(); break;
                case 15: conn.task15(); break;
                case 16: conn.task16(); break;
                case 17: conn.task17(); break;
                case 18: conn.task18(); break;
                case 19: conn.task19(); break;
                case 20: conn.task20(); break;
                case 21: conn.task21(); break;
                case 22: conn.task22(); break;
                case 23: conn.task23(); break;
                case 24: conn.task24(); break;
                case 25: conn.task25(); break;
                case 26: conn.task26(); break;
                case 27: conn.task27(); break;
                case 28: conn.task28(); break;
                case 29: conn.task29(); break;
                case 30: conn.task30(); break;
                case 31: conn.task31(); break;
                default:
                    std::string servString;
                    conn.in.read(servString);
                    std::cout << "got string: " << servString << std::endl;
                    break;
            }

        }
    } catch (std::exception& err) {
        std::cout << err.what() << std::endl;
    }

    conn.stop();

    TcpSocket::cleanupSockets();
    return 0;
}