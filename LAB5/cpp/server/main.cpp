#include <algorithm>
#include <iostream>
#include <queue>
#include <sstream>
#include <thread>
#include <vector>
#include "sqlite3.h"
#include <winsock2.h>
#include <windows.h>
#include "util.h"

class Car {
private:
    std::string name;
    int max_speed;
    int tank_capacity;
    int engine_power;
    int price;

public:
    Car(const char* name, const int max_speed, const int tank_capacity, const int engine_power,
        const int price) : name(name),
        max_speed(max_speed), tank_capacity(tank_capacity), engine_power(engine_power), price(price) {
    }

    std::string get_name() const {
        return name;
    }

    int get_max_speed() const {
        return max_speed;
    }

    int get_tank_capacity() const {
        return tank_capacity;
    }

    int get_engine_power() const {
        return engine_power;
    }

    int get_price() const {
        return price;
    }
};

struct Query_chg {
    std::string property;
    std::string value;
    std::string key;
};

struct Query {
    std::string property;
    std::string sign;
    std::string value;

    Query(const std::string& property, const std::string& sign, const std::string& value) : property(property),
        sign(sign),
        value(value) {
    }

    std::string get_property() const {
        return property;
    }

    std::string get_sign() const {
        return sign;
    }

    std::string get_value() const {
        return value;
    }
};

struct ClientConnection {
    SOCKET socket;
    bool has_notification;
    std::string notification;
};

std::vector<ClientConnection> clients;
HANDLE clients_mutex = CreateMutex(NULL, FALSE, NULL);

void notification_sender_thread() {
    while (true) {
        Sleep(100);
        WaitForSingleObject(clients_mutex, INFINITE);
        for (auto& client : clients) {
            if (client.has_notification) {
                send(client.socket, client.notification.c_str(),
                    client.notification.size(), NULL);
                client.has_notification = false;
            }
        }
        ReleaseMutex(clients_mutex);
    }
}

void broadcast(const std::string& message, SOCKET exclude = INVALID_SOCKET) {
    WaitForSingleObject(clients_mutex, INFINITE);
    for (auto& client : clients) {
        if (client.socket != exclude) {
            client.notification = "NOTIFY:" + message;
            client.has_notification = true;
        }
    }
    ReleaseMutex(clients_mutex);
}

bool save_db(sqlite3* db, const Car& car, const SOCKET sender_socket) {
    char* err;
    const std::string query = "INSERT INTO cars (name, max_speed, tank_capacity, engine_power, price) VALUES ('" +
        car.get_name() + "', " +
        std::to_string(car.get_max_speed()) + ", " +
        std::to_string(car.get_tank_capacity()) + ", " +
        std::to_string(car.get_engine_power()) + ", " +
        std::to_string(car.get_price()) + ");";
    int rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err << std::endl;
        sqlite3_free(err);
        return false;
    }
    broadcast("Added new car: " + car.get_name(), sender_socket);
    return true;
}

int del_db(sqlite3* db, const std::string& car_name, const SOCKET sender_socket) {
    char* err;
    const std::string query = "DELETE FROM cars WHERE name = '" + car_name + "'";
    int rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err << std::endl;
        sqlite3_free(err);
        return DELETE_ERROR;
    }
    const int deletedCount = sqlite3_changes(db);
    if (deletedCount != 0)
        broadcast("Deleted cars: " + car_name, sender_socket);

    return deletedCount;
}

bool change_db(sqlite3* db, const Query_chg& query_st, const SOCKET sender_socket) {
    char* err;
    const std::string query = "UPDATE cars SET " + query_st.property + " = '" + query_st.value + "'" + " WHERE name = "
        +
        "'" + query_st.key + "'";
    int rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err << std::endl;
        sqlite3_free(err);
        return false;
    }
    const int updatedCount = sqlite3_changes(db);
    if (updatedCount == 0)
        return true;
    broadcast("Updated cars: " + query_st.key + ' ' + query_st.property + " = " + query_st.value + '\0', sender_socket);
    return true;
}

static int callback(void* data, int argc, char** argv, char** colName) {
    std::vector<Car>* cars = static_cast<std::vector<Car>*>(data);

    std::string name = argv[1];
    int max_speed = std::stoi(argv[2]);
    int tank_capacity = std::stoi(argv[3]);
    int engine_power = std::stoi(argv[4]);
    int price = std::stoi(argv[5]);

    cars->emplace_back(name.c_str(), max_speed, tank_capacity, engine_power, price);
    return 0;
}

std::vector<Car> read_db(sqlite3* db, const Query& query_st) {
    char* err;
    std::vector<Car> cars;
    const std::string query = "SELECT * from cars where " + query_st.get_property() + query_st.get_sign() + "'" +
        query_st.
        get_value() + "'";
    int rc = sqlite3_exec(db, query.c_str(), callback, &cars, &err);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err << std::endl;
        sqlite3_free(err);
    }
    return cars;
}

void start_client() {
    const auto run_client = R"(start E:\lab5_client\cmake-build-debug\lab5_client.exe)";
    system(run_client);
}

int input_clients() {
    std::cout << "Enter an amount of clients (positive integer number between 1 and 11)\n";
    int res = 0;
    while (!(std::cin >> res) || std::cin.peek() != '\n' || res > 11 || res < 1) {
        std::cin.clear();
        while (std::cin.get() != '\n');
        std::cerr << "The amount of clients is incorrect\n";
        std::cerr << "Enter a positive integer number between 3 and 50\n";
    }
    return res;
}

bool open_db(sqlite3*& db) {
    int rc = sqlite3_open("main.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Не удалось открыть базу данных\n";
        return false;
    }
    return true;
}

void initialise_WSA() {
    WSAData wsaData{};
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData)) {
        std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
    }
}

HANDLE db_mutex = CreateMutex(NULL, FALSE, NULL);

void act_client(const int i, const std::vector<SOCKET>& sockets, sqlite3*& db) {
    SOCKET sock = sockets[i];
    char* buf = new char[BUFSIZE];
    while (true) {
        int len = recv(sock, buf, BUFSIZE, NULL);
        if (len == SOCKET_ERROR) {
            if (WSAGetLastError() == DISCONNECTED) {
                std::cout << "Client " << i + 1 << " disconnected" << std::endl;
                WaitForSingleObject(clients_mutex, INFINITE);
                clients.erase(std::remove_if(clients.begin(), clients.end(),
                    [sock](const ClientConnection& c) { return c.socket == sock; }),
                    clients.end());
                ReleaseMutex(clients_mutex);
                break;
            }
            std::cout << "Recv failed: " << WSAGetLastError() << std::endl;
            break;
        }
        if (!len) {
            std::cout << "Client " << i + 1 << " disconnected" << std::endl;
            break;
        }
        buf[len] = '\0';
        std::istringstream iss(buf);
        std::string oper_type;
        iss >> oper_type;
        if (oper_type == "select") {
            std::vector<Car> cars;
            std::string propety;
            std::string sign;
            std::string value;
            iss >> propety >> sign >> value;
            Query query(propety, sign, value);
            WaitForSingleObject(db_mutex, INFINITE);
            cars = read_db(db, query);
            ReleaseMutex(db_mutex);
            std::string car_prop;
            if (send(sock, std::to_string(cars.size()).c_str(), std::to_string(cars.size()).size(), NULL) ==
                SOCKET_ERROR) {
                std::cerr << "send failed: " << WSAGetLastError() << std::endl;
            }
            if (recv(sock, buf, BUFSIZE, NULL) == SOCKET_ERROR) {
                std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
            }
            std::cout << "Message from client " << i + 1 << ": " << buf << std::endl;
            for (const auto& car : cars) {
                car_prop = car.get_name() + ' ' + std::to_string(car.get_max_speed()) + ' ' +
                    std::to_string(car.get_tank_capacity()) + ' ' + std::to_string(car.get_engine_power()) + ' '
                    + std::to_string(car.get_price()) + '\0';
                if (send(sock, car_prop.c_str(), car_prop.size(), NULL) == SOCKET_ERROR) {
                    std::cerr << "send failed: " << WSAGetLastError() << std::endl;
                }
                Sleep(5);
                if (recv(sock, buf, BUFSIZE, NULL) == SOCKET_ERROR) {
                    std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
                }
                std::cout << "Message from client " << i + 1 << ": " << buf << std::endl;
            }
            std::cout << std::endl;
        }
        else if (oper_type == "add") {
            std::string name;
            int max_speed;
            char sign;
            std::string prop;
            int tank_capacity;
            int engine_power;
            int price;
            iss >> prop >> sign >> name >> prop >> sign >> max_speed >> prop >> sign >> tank_capacity >> prop >> sign >>
                price >> prop >> sign >> engine_power;
            Car car((name.c_str()), max_speed, tank_capacity, engine_power, price);
            WaitForSingleObject(db_mutex, INFINITE);
            if (!save_db(db, car, sock)) {
                if (send(sock, "Saving hasn't been done", LEN_MSG_SUC2, NULL) == SOCKET_ERROR) {
                    std::cerr << "send failed: " << WSAGetLastError() << std::endl;
                }
            }
            ReleaseMutex(db_mutex);
            if (send(sock, "Saving has been done", LEN_MSG_FAIL2, NULL) == SOCKET_ERROR) {
                std::cerr << "send failed: " << WSAGetLastError() << std::endl;
            }
        }
        else if (oper_type == "change") {
            std::string name;
            std::string propety;
            std::string value;
            std::string sign;
            iss >> name >> propety >> sign >> value;
            Query_chg query(propety, value, name);
            WaitForSingleObject(db_mutex, INFINITE);
            if (!change_db(db, query, sock)) {
                if (send(sock, "Changing hasn't been done", LEN_MSG_SUC1, NULL) == SOCKET_ERROR) {
                    std::cerr << "send failed: " << WSAGetLastError() << std::endl;
                }
            }
            ReleaseMutex(db_mutex);
            if (send(sock, "Changing has been done", LEN_MSG_FAIL1, NULL) == SOCKET_ERROR) {
                std::cerr << "send failed: " << WSAGetLastError() << std::endl;
            }
        }
        else if (oper_type == "del") {
            std::string name;
            std::string propety;
            std::string sign;
            iss >> propety >> sign >> name;
            WaitForSingleObject(db_mutex, INFINITE);
            int deleted;
            if ((deleted = del_db(db, name, sock)) == DELETE_ERROR) {
                if (send(sock, "Deleting hasn't been done", LEN_MSG_SUC1, NULL) == SOCKET_ERROR) {
                    std::cerr << "send failed: " << WSAGetLastError() << std::endl;
                }
            }
            ReleaseMutex(db_mutex);
            std::string resp;
            resp = "Deleting has been done. The " + std::to_string(deleted) + " have been deleted.";
            if (send(sock, resp.c_str(), BUFSIZE, NULL) == SOCKET_ERROR) {
                std::cerr << "send failed: " << WSAGetLastError() << std::endl;
            }
        }
    }
    delete[] buf;
}

void backend(const int n, const std::vector<SOCKET>& connections, std::vector<std::thread>& client_threads,
    sqlite3*& db) {
    for (int i = 0; i < n; ++i) {
        if (connections[i] != INVALID_SOCKET) {
            client_threads.emplace_back(act_client, i, std::ref(connections), std::ref(db));
        }
    }
    for (auto& thread : client_threads) {
        if (thread.joinable())
            thread.join();
    }
    std::cout << "All clients disconnected.\n";
}

int main() {
    initialise_WSA();
    SOCKADDR_IN addr;
    int size = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;

    SOCKET sock = socket(AF_INET, SOCK_STREAM, NULL);
    if (bind(sock, (SOCKADDR*)&addr, size) == SOCKET_ERROR) {
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
    SOCKET new_connection;
    std::vector<SOCKET> connections;
    const int n_clients = input_clients();
    int id;
    std::thread notification_thread(notification_sender_thread);
    notification_thread.detach();
    for (int i = 0; i < n_clients; i++) {
        id = i;
        start_client();
        new_connection = accept(sock, (SOCKADDR*)&addr, &size);
        if (!new_connection) {
            std::cerr << "Accept for client " << id << " failed with error: " << WSAGetLastError() << std::endl;
            if (closesocket(sock) == SOCKET_ERROR) {
                std::cerr << "Failed to terminate connection: " << WSAGetLastError() << std::endl;
            }
            continue;
        }
        connections.push_back(new_connection);
        WaitForSingleObject(clients_mutex, INFINITE);
        clients.push_back({ new_connection, false, "" });
        ReleaseMutex(clients_mutex);
        std::cout << "Client " << id + 1 << " connected\n";
    }
    sqlite3* db = nullptr;
    open_db(db);
    std::vector<std::thread> client_threads;
    backend(n_clients, connections, client_threads, db);
    for (auto& conn : connections) {
        closesocket(conn);
    }
    closesocket(sock);
    CloseHandle(clients_mutex);
    CloseHandle(db_mutex);
    WSACleanup();
}