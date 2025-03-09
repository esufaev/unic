#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

void receive_messages(int client_socket)
{
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0)
        {
            break;
        }
        buffer[bytes_received] = '\0';
        std::cout << buffer << std::endl;
    }
}

int main()
{
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(client_socket, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cerr << "Connection failed!" << std::endl;
        return 1;
    }

    char nickname[50];
    while (true)
    {
        std::cout << "Enter your nickname: ";
        std::cin.getline(nickname, sizeof(nickname));
        send(client_socket, nickname, strlen(nickname), 0);

        char response[1024];
        memset(response, 0, sizeof(response));
        recv(client_socket, response, sizeof(response) - 1, 0);
        response[strlen(response)] = '\0';
        if (std::string(response) != "Nickname taken or banned. Choose another.")
        {
            std::cout << response << std::endl;
            break;
        }
        std::cout << response << std::endl;
    }

    std::thread(receive_messages, client_socket).detach();

    char buffer[1024];
    while (true)
    {
        std::cin.getline(buffer, sizeof(buffer));
        send(client_socket, buffer, strlen(buffer), 0);
    }

    close(client_socket);
    return 0;
}
