#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

std::unordered_map<std::string, int> clients;
std::unordered_set<std::string> banned_users;
std::mutex clients_mutex;

void handle_client(int client_socket)
{
    char buffer[1024];
    char nickname[50];
    memset(nickname, 0, sizeof(nickname));

    int bytes_received = recv(client_socket, nickname, sizeof(nickname) - 1, 0);
    if (bytes_received <= 0)
    {
        close(client_socket);
        return;
    }
    nickname[bytes_received] = '\0';

    std::string nick_str(nickname);
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        if (clients.find(nick_str) != clients.end() || banned_users.find(nick_str) != banned_users.end())
        {
            const char *msg = "Nickname taken or banned. Choose another.";
            send(client_socket, msg, strlen(msg), 0);
            close(client_socket);
            return;
        }
        clients[nick_str] = client_socket;
    }

    std::string welcome_msg = "Welcome " + nick_str + " to the chat!";
    send(client_socket, welcome_msg.c_str(), welcome_msg.size(), 0);
    std::cout << nick_str << " joined the chat." << std::endl;

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0)
        {
            break;
        }
        buffer[bytes_received] = '\0';

        std::string message = nick_str + ": " + buffer;
        std::cout << message << std::endl;

        std::lock_guard<std::mutex> lock(clients_mutex);
        for (const auto &[nick, client] : clients)
        {
            if (client != client_socket)
            {
                send(client, message.c_str(), message.size(), 0);
            }
        }
    }

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.erase(nick_str);
    }

    close(client_socket);
    std::cout << nick_str << " left the chat." << std::endl;
}

void ban_user(const std::string &nick)
{
    std::lock_guard<std::mutex> lock(clients_mutex);
    auto it = clients.find(nick);
    if (it != clients.end())
    {
        close(it->second);
        clients.erase(it);
        banned_users.insert(nick);
        std::cout << nick << " has been banned." << std::endl;
    }
}

int main()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, 5);

    std::cout << "Server started on port 8080..." << std::endl;

    std::thread([]()
                {
        std::string command;
        while (true)
        {
            std::cin >> command;
            if (command == "/ban")
            {
                std::string nick;
                std::cin >> nick;
                ban_user(nick);
            }
        } })
        .detach();

    while (true)
    {
        sockaddr_in client_addr;
        socklen_t client_size = sizeof(client_addr);
        int client_socket = accept(server_socket, (sockaddr *)&client_addr, &client_size);
        if (client_socket != -1)
        {
            std::thread(handle_client, client_socket).detach();
        }
    }

    close(server_socket);
    return 0;
}