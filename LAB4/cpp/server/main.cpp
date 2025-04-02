#include <chrono>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#define BUFSIZE 1024
#define SIZE_MSG 30
SOCKET Connections[51];

void start_client() {
    const auto run_client = R"(start E:\client\cmake-build-debug\client.exe)";
    system(run_client);
}

bool all_disc(const int n) {
    return std::all_of(Connections + 1, Connections + n + 1, [](const SOCKET i) { return i == INVALID_SOCKET; });
}

int input_clients() {
    std::cout << "Enter an amount of clients (positive integer number between 3 and 50)\n";
    int res = 0;
    while (!(std::cin >> res) || std::cin.peek() != '\n' || res > 11 || res < 3) {
        std::cin.clear();
        while (std::cin.get() != '\n');
        std::cerr << "The amount of clients is incorrect\n";
        std::cerr << "Enter a positive integer number between 3 and 50\n";
    }
    return res;
}

void sending(const std::string &msg, const int i) {
    if (send(Connections[i], msg.c_str(), msg.size() + 1, NULL) == SOCKET_ERROR) {
        std::cout << "Client " << i << " disconnected." << std::endl;
        closesocket(Connections[i]);
        Connections[i] = INVALID_SOCKET;
    }
}

void monitorClients(const int n) {
    int result;
    fd_set readSet;
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1;
    for (int i = 1; i <= n; ++i)
        sending("\nAre you in a process?", i);

    while (true) {
        FD_ZERO(&readSet);
        for (int i = 1; i <= n; ++i)
            if (Connections[i] != INVALID_SOCKET)
                FD_SET(Connections[i], &readSet);
        int activity = select(0, &readSet, nullptr, nullptr, &timeout);
        if (activity == SOCKET_ERROR)
            std::cout << "Select failed." << GetLastError() << std::endl;
        if (activity > 0) {
            for (int i = 1; i <= n; ++i) {
                if (Connections[i] != SOCKET_ERROR && FD_ISSET(Connections[i], &readSet)) {
                    auto buffer = new char[BUFSIZE + 1];
                    result = recv(Connections[i], buffer, BUFSIZE, NULL);
                    if (result == SOCKET_ERROR) {
                        std::cout << "Client " << i << " disconnected." << std::endl;
                        closesocket(Connections[i]);
                        Connections[i] = INVALID_SOCKET;
                        continue;
                    }
                    buffer[result] = '\0';
                    std::cout << "Message from client " << i << ": " << buffer << std::endl;
                    sending("Are you in a process?", i);
                    delete[] buffer;
                }
            }
        }
        if (all_disc(n)) {
            std::cout << "All clients disconnected.\n";
            return;
        }
    }
}

int main() {
    WSAData wsaData{};
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
        return 1;
    }

    SOCKADDR_IN addr;
    int size = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;

    SOCKET sock = socket(AF_INET, SOCK_STREAM, NULL);
    if (bind(sock, (SOCKADDR *) &addr, size) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    int id;
    char msg[256];
    const int n = input_clients();
    SOCKET new_connection;

    for (int i = 1; i <= n; ++i) {
        id = i;
        start_client();
        new_connection = accept(sock, (SOCKADDR *) &addr, &size);
        if (!new_connection) {
            std::cerr << "Accept for client " << id << " failed with error: " << WSAGetLastError() << std::endl;
            if (closesocket(sock) == SOCKET_ERROR) {
                std::cerr << "Failed to terminate connection: " << WSAGetLastError() << std::endl;
            }
            continue;
        }
        int res = recv(new_connection, msg, SIZE_MSG, NULL);
        std::string num = std::to_string(i);
        send(new_connection, num.c_str(), num.size() + 1, NULL);
        if (res == SOCKET_ERROR) {
            std::cerr << "Failed to receive message: " << WSAGetLastError() << std::endl;
        }
        std::cout << "Message from client " << id << ": " << msg << std::endl;
        u_long mode = 1;
        if (ioctlsocket(new_connection, FIONBIO, &mode) == SOCKET_ERROR) {
            std::cerr << "Failed to set non-blocking mode for client " << id << ": " << WSAGetLastError() <<
                    std::endl;
            closesocket(new_connection);
            continue;
        }
        Connections[i] = new_connection;
        std::cout << "Client " << i << " connected." << std::endl;
    }
    monitorClients(n);
    closesocket(sock);
    WSACleanup();
}
