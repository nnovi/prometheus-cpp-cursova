#include <iostream>
#include <vector>
#include <thread>
#include <csignal>
#include <cmath>
#include <latch>
#include <map>
#include "../common/App.h"
#include "../common/proto_buffer.h"
#include "../common/tcp_server_socket.h"
#include "../common/utils.h"

#ifndef WIN32
#include <poll.h>
#endif

std::string TASK1_SURNAME = "Novychenko";
double TASK5_CONST = 50;
std::string TASK11_NAME = "Mykola";

struct ClientConn {
    int ID;
    SOCKET sock;
    TcpSocket* conn;
    ProtoBuffer out;
    ProtoBuffer in;
    std::thread* handler;

    ~ClientConn() {
        if (conn != nullptr) {
            delete conn;
            conn = nullptr;
        }
    }

    void close() {
        if (conn != nullptr) {
            conn->closeConnection();
        }
        out.close();
        in.close();
        if (handler != nullptr) {
            handler->join();
        }
    }

    void start_handler() {
        this->handler = new std::thread(&ClientConn::proto_handler, this);
    }

    void proto_handler() {
        try {
            while (true) {
                int variant;
                in.read(variant);
                if (variant <= 0 || variant > 31) {
                    break;
                }
                std::cout << "[client #" << ID << "] got variant " << variant << std::endl;

                switch (variant) {
                    case 1: task1(); break;
                    case 2: task2(); break;
                    case 3: task3(); break;
                    case 4: task4(); break;
                    case 5: task5(); break;
                    case 6: task6(); break;
                    case 7: task7(); break;
                    case 8: task8(); break;
                    case 9: task9(); break;
                    case 10: task10(); break;
                    case 11: task11(); break;
                    case 12: task12(); break;
                    case 13: task13(); break;
                    case 14: task14(); break;
                    case 15: task15(); break;
                    case 16: task16(); break;
                    case 17: task17(); break;
                    case 18: task18(); break;
                    case 19: task19(); break;
                    case 20: task20(); break;
                    case 21: task21(); break;
                    case 22: task22(); break;
                    case 23: task23(); break;
                    case 24: task24(); break;
                    case 25: task25(); break;
                    case 26: task26(); break;
                    case 27: task27(); break;
                    case 28: task28(); break;
                    case 29: task29(); break;
                    case 30: task30(); break;
                    case 31: task31(); break;
                    default:
                        out.write("not implemented");
                        break;
                }
            }
        } catch (std::exception& err) {}

        if (conn != nullptr) conn->closeConnection();
        in.close();
        out.close();
    }

    void task1() {
        std::string name;
        in.read(name);
        out.write(name + " " + TASK1_SURNAME);
    }

    void task2() {
        double a, b, result;
        in.read(a);
        in.read(b);
        result = a + b;
        out.write(result);
    }

    void task3() {
        double a, b, result;
        a = rand() % 10000;
        b = rand() % 10000;
        out.write(a);
        out.write(b);
        in.read(result);
        std::cout << "[client #" << ID << "] returned " << result << ", correct result is " << (a*b) << std::endl;
    }

    void task4() {
        std::vector<double> arr;
        in.read(arr);
        double result = 0;
        if (!arr.empty()) {
            result = arr[0];
            for (int i=1; i<arr.size(); i++) {
                if (result < arr[i]) {
                    result = arr[i];
                }
            }
        }
        out.write(result);
    }

    void task5() {
        double x;
        in.read(x);
        if (x > TASK5_CONST) x = TASK5_CONST;
        out.write(x);
    }

    void task6() {
        int v;
        in.read(v);
        std::string s = std::to_string(v);

        bool result = true;
        for (int i=0; i<s.size()/2; i++) {
            if (s[i] != s[s.size()-1-i]) {
                result = false;
                break;
            }
        }
        out.write(result ? "Yes" : "No");
    }

    void task7() {
        std::vector<double> arr;
        int sz = 8 + (rand() % 32);
        for (int i = 0; i < sz; i++) {
            arr.push_back(rand() % 10000);
        }
        out.write(arr);
        std::cout << "[client #" << ID << "] task: " << arr << std::endl;

        std::vector<double> result;
        in.read(result);
        std::cout << "[client #" << ID << "] result: " << result << std::endl;
    }

    void task8() {
        std::string s;
        in.read(s);
        int result = 0;
        for (auto it : s) {
            if (it == 'a') {
                result++;
            }
        }
        out.write(result);
    }

    void task9() {
        std::string s = random_string();
        out.write(s);
        std::cout << "[client #" << ID << "] task: " << s << std::endl;
        in.read(s);
        std::cout << "[client #" << ID << "] result: " << s << std::endl;
    }

    void task10() {
        std::string s;
        in.read(s);
        bool result = false;
        for (auto it : s) {
            if (it >= '0' && it <= '9') {
                result = true;
                break;
            }
        }
        out.write(result ? "Yes":"No");
    }

    void task11() {
        std::string s;
        in.read(s);
        s = TASK11_NAME + " " + s;
        out.write(s);
    }

    void task12() {
        double a, b, c, result;
        in.read(a);
        in.read(b);
        in.read(c);
        result = a + b + c;
        out.write(result);
    }

    void task13() {
        double a = (rand() % 1000) - 500;
        double b = (rand() % 1000) - 500;
        double c = (rand() % 1000) - 500;
        out.write(a);
        out.write(b);
        out.write(c);
        std::cout << "[client #" << ID << "] task: " << a << ", " << b << ", " << c << std::endl;
        double result;
        in.read(result);
        std::cout << "[client #" << ID << "] result: " << result << std::endl;
    }

    void task14() {
        std::vector<double> arr;
        in.read(arr);

        double min = 0, max = 0;
        if (!arr.empty()) {
            min = arr[0];
            max = min;
            for (int i = 1; i < arr.size(); i++) {
                double v = arr[i];
                if (min > v) min = v;
                if (max < v) max = v;
            }
        }
        out.write(min);
        out.write(max);
    }

    void task15() {
        double a,b,c,d,avg;
        in.read(a);
        in.read(b);
        in.read(c);
        in.read(d);
        avg = (a+b+c+d)/4;
        a += avg;
        b += avg;
        c += avg;
        d += avg;
        out.write(a);
        out.write(b);
        out.write(c);
        out.write(d);
    }

    void task16() {
        int x;
        bool result;
        in.read(x);

        if (x < 0) x = -x;
        if (x % 2 == 0) {
            result = false;
        } else {
            result = true;
            for (int i = 3; i <= x/2; i+= 2) {
                if (x % i == 0) {
                    result = false;
                    break;
                }
            }
        }
        out.write(result ? "Yes" : "No");
    }

    void task17() {
        std::vector<double> arr;
        int sz = 8 + (rand() % 32);
        for (int i = 0; i < sz; i++) {
            arr.push_back(rand() % 10000);
        }
        out.write(arr);
        std::cout << "[client #" << ID << "] task: " << arr << std::endl;

        std::vector<double> result;
        in.read(result);
        std::cout << "[client #" << ID << "] result: " << result << std::endl;
    }

    void task18() {
        std::string s;
        in.read(s);
        int result = 0;
        for (auto it : s) {
            if(it == ' ') result++;
        }
        out.write(result);
    }

    void task19() {
        std::string s = random_string();
        out.write(s);
        std::cout << "[client #" << ID << "] task: " << s << std::endl;
        in.read(s);
        std::cout << "[client #" << ID << "] result: " << s << std::endl;
    }

    void task20() {
        std::string s;
        in.read(s);
        for (auto it : s) {
            if(it == ' ') {
                out.write("Yes");
                return;
            }
        }
        out.write("No");
    }

    void task21() {
        int num;
        std::vector<int> result;
        in.read(num);

        if (num < 0) {
            num = -num;
            result.push_back(-1);
        }
        if (num == 0) {
            result.push_back(0);
        } else if (num == 1) {
            if (result.empty()) result.push_back(1);
        } else {
            for (int i=2; i<num/2 + 1;) {
                if (num % i == 0) {
                    result.push_back(i);
                    num /= i;
                    if (num == 1) {
                        break;
                    }
                } else {
                    ++i;
                }
            }
            if (num != 1) {
                result.push_back(num);
            }
        }
        out.write(result);
    }

    void task22() {
        int num;
        in.read(num);

        auto sqn = (int)std::sqrt(num);
        if (sqn*sqn != num)
            out.write("No");
        else
            out.write("Yes");
    }

    void task23() {
        std::vector<double> arr;
        int sz = 8 + (rand() % 32);
        for (int i = 0; i < sz; i++) {
            arr.push_back((rand() % 10000) - 5000);
        }
        out.write(arr);
        std::cout << "[client #" << ID << "] task: " << arr << std::endl;

        std::vector<double> result;
        in.read(result);
        std::cout << "[client #" << ID << "] result: " << result << std::endl;
    }

    void task24() {
        std::string s;
        in.read(s);

        int result = 0;
        for (auto it : s) {
            switch (it) {
                case 'A':
                case 'E':
                case 'I':
                case 'O':
                case 'U':
                case 'Y':
                case 'a':
                case 'e':
                case 'i':
                case 'o':
                case 'u':
                case 'y':
                    result++;
                    break;
            }
        }
        out.write(result);
    }

    void task25() {
        std::string s = random_string();
        out.write(s);
        std::cout << "[client #" << ID << "] task: " << s << std::endl;

        std::vector<int> result;
        in.read(result);
        std::cout << "[client #" << ID << "] result: " << s << std::endl;
    }

    void task26() {
        std::string s;
        in.read(s);

        for (auto it : s) {
            if (std::ispunct(it)) {
                out.write("Yes");
                return;
            }
        }
        out.write("No");
    }

    void task27() {
        std::string s;
        in.read(s);
        std::erase_if(s, [](char c) -> bool {
            return c >= 'A' && c <= 'Z';
        });
        out.write(s);
    }

    void task28() {
        double a,b,c;
        in.read(a);
        in.read(b);
        in.read(c);

        double result = pow(a*b*c, 1/3.0);
        out.write(result);
    }

    void task29() {
        bool eq = rand() % 3 == 0;
        std::string s1 = random_string();
        std::string s2 = eq ? s1 : random_string();

        out.write(s1);
        out.write(s2);

        std::string result;
        in.read(result);
        std::cout << "[client #" << ID << "] task: s1=" << s1 << std::endl;
        std::cout << "[client #" << ID << "] task: s2=" << s2 << std::endl;
        std::cout << "[client #" << ID << "] result: " << result << std::endl;
    }

    void task30() {
        std::vector<double> arr;
        double result = 1;
        in.read(arr);
        for (auto it : arr) {
            result *= it;
        }
        out.write(result);
    }

    void task31() {
        int x;
        while (true) {
            in.read(x);
            if (x == -1) break;
            out.write(x);
        }
    }

    static std::string random_string() {
        std::string s;
        int sz = 4 + rand() % 32;
        for (int i = 0; i<sz; i++) {
            s.push_back('A' + (rand() % ('Z' - 'A' - 1)));
        }
        return s;
    }
};

class ServerApp : public App, public IProtoBufferListener {
public:
    ServerApp(const std::string& ip_serv, int port): _host(ip_serv), _port(port) {}

    ~ServerApp() {
        if (_sock != nullptr) {
            delete _sock;
            _sock = nullptr;
        }
    }

    void protobuffer_updated() override {
        std::lock_guard<std::recursive_mutex> lock(_mut);
        _buffer_updated = true;
    }

protected:
    void run() {
        init_socket();
        main_loop();
        std::cout << "server closed" << std::endl;
    }

private:
    TcpServerSocket* _sock = nullptr;
    std::string _host;
    int _port;
    std::recursive_mutex _mut;
    volatile bool _buffer_updated = false;

    void init_socket() {
        _sock = new TcpServerSocket(_host.c_str(), _port);
        _port = _sock->getPort();
    }

    void main_loop() {
        std::map<SOCKET, ClientConn*> clients;
#ifdef WIN32
        std::vector<WSAPOLLFD> poll_fd;
        poll_fd.push_back(WSAPOLLFD{
            .fd =  _sock->getSocket(),
            .events = POLLRDNORM,
        });
#else
        std::vector<pollfd> poll_fd;
        poll_fd.push_back(pollfd{
            .fd = _sock->getSocket(),
            .events = POLLRDNORM,
        });
#endif

        int recv_buf_size = 4096;
        char recv_buf[recv_buf_size];
        int next_client_id = 1;

        std::cout << "server started. host=[" << _host << "] port=[" << _port << "]" << std::endl;
        std::cout << "use CTRL+C to interrupt" << std::endl;
        while (!is_interrupted()) {
            if (_buffer_updated) {
                std::lock_guard<std::recursive_mutex> lock(_mut);
                for (int i = 1; i<poll_fd.size();) {
                    ClientConn* client = clients[poll_fd[i].fd];
                    bool close = false;
                    if (client == nullptr) {
                        std::cerr << "warning: poll_fd contains socket missing in clients map" << std::endl;
                        close = true;
                    } else {
                        if (client->out.is_closed() || client->in.is_closed()) {
                            close = true;
                        } else {
                            if (client->out.data().empty()) {
                                poll_fd[i].events = POLLRDNORM;
                            } else {
                                poll_fd[i].events = POLLRDNORM | POLLWRNORM;
                            }
                        }
                    }

                    if (close) {
                        poll_fd.erase(poll_fd.begin()+i);
                        if (client != nullptr) {
                            clients.erase(client->sock);
                            client->close();
                            std::cout << "client #" << client->ID << " disconnected. active: " << clients.size() << std::endl;
                            delete client;
                        }
                    } else {
                        i++;
                    }
                }
                _buffer_updated = false;
            }

#ifdef WIN32
            int fd_ready = WSAPoll(poll_fd.data(), poll_fd.size(), 1);
#else
            int fd_ready = poll(poll_fd.data(), poll_fd.size(), 1);
#endif
            if (fd_ready <= 0) {
                // no sockets ready, just continue
                continue;
            }

            if (poll_fd[0].revents & POLLRDNORM) {
                fd_ready--;
                poll_fd[0].revents = 0;

                // server socket has new connection, accept it
                TcpSocket* clientConn = _sock->acceptConnection();
                if (clientConn == nullptr) {
                    std::cerr << "error accepting new socket: " << _sock->getMessage() << std::endl;
                } else {
                    auto client = new ClientConn{
                            .ID = next_client_id++,
                            .sock = clientConn->getSocket(),
                            .conn = clientConn,
                    };
                    clients[client->sock] = client;
                    client->out.set_listener(this);
                    client->start_handler();
#ifdef WIN32
                    poll_fd.push_back(WSAPOLLFD{
                            .fd =  client->sock,
                            .events = POLLRDNORM,
                    });
#else
                    poll_fd.push_back(pollfd{
                            .fd =  client->sock,
                            .events = POLLRDNORM,
                    });
#endif
                    std::cout << "client #" << client->ID << " connected from " << clientConn->getHost() << ":" << clientConn->getPort() << ". active clients: " << clients.size() << std::endl;
                }
            }

            if (fd_ready > 0) {
                for (auto it = poll_fd.begin() + 1; it != poll_fd.end(); ) {
                    bool remove_fd = false;
                    if (it->revents != 0) {
                        ClientConn *client = clients[it->fd];
                        if (it->revents & (POLLHUP|POLLERR)) {
                            remove_fd = true;
                        } else {
                            if (it->revents & POLLWRNORM) {
                                try {
                                    client->out.send(*client->conn);
                                } catch (std::exception& err) {
                                    std::cerr << "error writing to client: " << err.what() << std::endl;
                                    remove_fd = true;
                                }
                            }
                            if (it->revents & POLLRDNORM) {
                                // new data received on client connection
                                try {
                                    int sz = client->in.recv(*client->conn);
                                    if (sz == 0) {
                                        remove_fd = true;
                                    }
                                } catch (std::exception& err) {
                                    std::cerr << "error processing client data: " << err.what() << std::endl;
                                    remove_fd = true;
                                }
                            }
                        }

                        if (remove_fd) {
                            std::cout << "client #" << client->ID << " disconnected" << std::endl;
                            clients.erase(client->sock);
                            client->close();
                            delete client;
                            it = poll_fd.erase(it);
                        } else {
                            it->revents = 0;
                            ++it;
                        }
                        fd_ready--;
                        if (fd_ready == 0) break;
                    } else {
                        ++it;
                    }
                }
            }
        }
        std::cout << "closing all connections..." << std::endl;
        for (auto it : clients) {
            it.second->close();
        }
    }
};

void print_usage(const std::string& cmd, const std::string& host, int port) {
    std::cout << "Usage: " << std::endl
              << cmd << " ||| Using default host/port: " << host << ":" << port << std::endl
              << cmd << " {PORT} ||| Using default host " << host << " with custom port" << std::endl
              << cmd << " {HOST} {PORT} ||| Using custom host and port" << std::endl;
}

int main(int argc, const char* argv[]) {
    std::string IP_SERV = "0.0.0.0";       // local Server IP address
    int PORT_NUM = 25000;             // working server port

    if (argc > 3) {
        print_usage(argv[0], IP_SERV, PORT_NUM);
        exit(1);
    } else if (argc == 2) {
        PORT_NUM = atoi(argv[1]);
    } else if (argc == 3) {
        PORT_NUM = atoi(argv[2]);
        IP_SERV = argv[1];
    }

    ServerApp app(IP_SERV, PORT_NUM);
    app.start();
    return 0;
}
