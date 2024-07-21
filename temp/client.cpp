#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"

void send_message(int socket, const std::string &message)
{
    int length = htonl(message.size());
    write(socket, &length, sizeof(int));
    write(socket, message.c_str(), message.size());
}

std::string receive_message(int socket)
{
    int length;
    read(socket, &length, sizeof(int));
    length = ntohl(length);

    char buffer[BUFFER_SIZE] = {0};
    read(socket, buffer, length);

    return std::string(buffer, length);
}

int main()
{
    int client_fd;
    struct sockaddr_in server_address;

    // Creating socket file descriptor
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Socket creation failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        std::cerr << "Connection Failed" << std::endl;
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // Send HELLO message
    send_message(client_fd, "HELLO");
    std::cout << "Server response: " << receive_message(client_fd) << std::endl;

    // Send ECHO message
    send_message(client_fd, "ECHO This is a test message");
    std::cout << "Server response: " << receive_message(client_fd) << std::endl;

    // Send BYE message
    send_message(client_fd, "BYE");
    std::cout << "Server response: " << receive_message(client_fd) << std::endl;

    // Close the socket
    close(client_fd);

    return 0;
}
