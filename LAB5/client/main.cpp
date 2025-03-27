#include<iostream>
#include<winsock2.h>
#include <regex>
#include <thread>
#include <conio.h>
#include "util.h"
bool running = true;

void initialise_WSA() {
    WSAData wsaData{};
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData)) {
        std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
    }
}

void inf_msg() {
    std::cout << "You are allowed to choose cars by its properties and change its properties\n";
    std::cout << "Here are the properties of cars\n";
    std::cout << "name\nmax_speed\ntank_capacity\nprice\nengine_power\n";
    std::cout << "Every token must be divided by space\n";
    std::cout << "---------------------------------------------------------------------\n";
    std::cout << "To get information about the car you should enter 'select' and the name of the car\n";
    std::cout <<
            "If you need to get some cars by it property you should enter the name of the property, the sign(<,<=,=,>,>=) and the value of the property\n";
    std::cout << "For the name is allowed only =\n";
    std::cout << "---------------------------------------------------------------------\n";
    std::cout <<
            "To change information about the car you should enter 'change', the name of the car, changeable property, '=' and the new value of the property\n";
    std::cout << "--------------------------------------------------------------------\n";
    std::cout << "To add new car you should enter 'add' and all properties of the car\n";
    std::cout << "--------------------------------------------------------------------\n";
    std::cout << "To delete car you should enter 'del' 'name' and the name of the car\n";
}

std::string oper_choice() {
    std::string choice;
    std::regex pattern_select(
        R"(^select ((name = \w+)|(max_speed|tank_capacity|engine_power|price) (<|<=|=|>|>=) \d+$))");
    std::regex pattern_add(R"(^add name = \w+ max_speed = \d+ tank_capacity = \d+ price = \d+ engine_power = \d+$)");
    std::regex pattern_change(
        R"(^change \w+ ((name = \w+)|(max_speed = \d+)|(tank_capacity = \d+)|(engine_power = \d+)|(price = \d+))$)");
    std::regex pattern_delete(R"(^del name = \w+$)");

    while (running) {
        if (_kbhit()) {
            std::getline(std::cin, choice);

            if (choice == "quit") {
                running = false;
                return choice;
            }

            if (std::regex_match(choice, pattern_select) ||
                std::regex_match(choice, pattern_add) ||
                std::regex_match(choice, pattern_change) ||
                std::regex_match(choice, pattern_delete)) {
                return choice;
            }
            std::cout << "Please enter a valid choice\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return "quit";
}

void message_receiver(const SOCKET sock) {
    char buf[BUFSIZE];
    while (running) {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(sock, &readSet);

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = TIME;

        int result = select(0, &readSet, NULL, NULL, &timeout);

        if (result > 0 && FD_ISSET(sock, &readSet)) {
            int len = recv(sock, buf, BUFSIZE, NULL);
            if (len <= 0) {
                running = false;
                break;
            }
            buf[len] = '\0';
            std::cout << buf << std::endl;
        }
    }
}

int main() {
    initialise_WSA();
    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;
    SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);
    if (connect(Connection, (SOCKADDR *) &addr, sizeof(addr))) {
        std::cerr << "Error: failed connect to the server.\n";
        return 1;
    }
    inf_msg();
    std::thread receiver(message_receiver, Connection);
    char *buf = new char[BUFSIZE];
    std::string choice = oper_choice();
    while (choice != "quit" && running) {
        if (send(Connection, choice.c_str(), choice.size(), NULL) == INVALID_SOCKET) {
            std::cerr << "Error: failed send to the server." << GetLastError() << std::endl;
            delete[] buf;
            running = false;
            break;
        }
        std::istringstream iss(choice);
        std::string oper;
        iss >> oper;
        if (oper == "select") {
            if (recv(Connection, buf, BUFSIZE, NULL) == SOCKET_ERROR) {
                std::cerr << "Error: failed recv from the server." << GetLastError() << std::endl;
                delete[] buf;
                running = false;
                break;
            }
            if (send(Connection, "Data has been accept", LEN_MSG, NULL) == SOCKET_ERROR) {
                std::cerr << "Error: failed send to the server." << GetLastError() << std::endl;
            }
            int n_cars = std::stoi(buf);
            if (n_cars == 0)
                std::cout << "There aren't any car";
            for (int i = 0; i < n_cars; ++i) {
                {
                    if (recv(Connection, buf, BUFSIZE, NULL) == SOCKET_ERROR) {
                        std::cerr << "Error: failed recv from the server." << GetLastError() << std::endl;
                    }
                    if (send(Connection, "Data has been accept", LEN_MSG, NULL) == SOCKET_ERROR) {
                        std::cerr << "Error: failed send to the server." << GetLastError() << std::endl;
                    }
                }
                std::cout << std::endl << buf;
            }
            std::cout << std::endl;
        } else if (oper == "add" || oper == "change" || oper == "del") {
            if (recv(Connection, buf, BUFSIZE, NULL) == SOCKET_ERROR) {
                std::cerr << "Error: failed recv from the server." << GetLastError() << std::endl;
            }
            std::cout << buf << std::endl;
        }
        choice = oper_choice();
    }
    running = false;
    if (receiver.joinable()) {
        receiver.join();
    }
    delete[] buf;
    closesocket(Connection);
    WSACleanup();
    return 0;
}
