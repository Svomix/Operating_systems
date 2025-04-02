#include <iostream>
#include<winsock2.h>
#define BUFFSIZE 1024

bool printCausedBy(int Result, const char *nameOfOper) {
    if (!Result) {
        std::cout << "Connection closed.\n";
        return false;
    }
    if (Result < 0) {
        std::cout << nameOfOper;
        std::cerr << " failed:\n", WSAGetLastError();
        return false;
    }
    return true;
}

bool sending(const SOCKET &connection, const std::string &statement) {
    if (send(connection, statement.c_str(), statement.size(), NULL) == SOCKET_ERROR) {
        std::cerr << "Send failed\n";
        return false;
    }
    return true;
}

int main() {
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        std::cerr << "Error: failed to link library.\n";
        return 1;
    }
    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;
    SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);

    if (connect(Connection, (SOCKADDR *) &addr, sizeof(addr))) {
        std::cerr << "Error: failed connect to the server.\n";
        return 1;
    }
    char *buffer_input = new char[BUFFSIZE];
    std::string ans;
    if (!sending(Connection, "Server connection established")) {
        delete[] buffer_input;
        return 1;
    }
    recv(Connection, buffer_input, BUFFSIZE, NULL);
    std::cout << buffer_input;
    while (printCausedBy(recv(Connection, buffer_input, BUFFSIZE, NULL), "Received data from server.")) {
        std::cout << buffer_input << std::endl;
        std::cin >> ans;
        if (!sending(Connection, ans)) {
            delete[] buffer_input;
            return 1;
        }
    }
    delete[] buffer_input;
    std::cout << "Server disconnected.\n";
    if (closesocket(Connection) == SOCKET_ERROR)
        std::cerr << "Error: failed to close socket.\n";
    WSACleanup();
}
