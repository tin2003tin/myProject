#ifndef SEND_HPP
#define SEND_HPP
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "echoProtocol.hpp"
#include "echoHandler.hpp"

class Conn
{
public:
    int createConnection(const std::string &server_address, int server_port)
    {
        int sock = 0;
        struct sockaddr_in serv_addr;

        // Create socket
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            std::cerr << "Socket creation error" << std::endl;
            return -1;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(server_port);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, server_address.c_str(), &serv_addr.sin_addr) <= 0)
        {
            std::cerr << "Invalid address/ Address not supported" << std::endl;
            return -1;
        }

        // Connect to server
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            std::cerr << "Connection Failed" << std::endl;
            return -1;
        }

        return sock;
    }

    void closeConnection(int sock)
    {
        close(sock);
    }

    std::string fetch(int sock, tin::MessageType message_type, const std::string &message_body)
    {
        // Create and send message
        tin::ProtocolMessage message(message_type, message_body);
        std::string serializedMessage = message.serialize();
        send(sock, serializedMessage.c_str(), serializedMessage.size(), 0);
        // std::cout << "Message sent: " << message.get_body() << std::endl;

        // Receive response
        char buffer[BUFFER_SIZE] = {0};
        int read_val = read(sock, buffer, BUFFER_SIZE);
        std::string response_body = "";
        if (read_val > 0)
        {
            tin::IProtocol *response = tin::ProtocolMessage::deserialize(buffer, read_val);
            response_body = response->get_body();
            delete response;
        }

        return response_body;
    }
};

// int main()
// {
//     // Create connection
//     int sock = createConnection(SERVER_ADDRESS, SERVER_PORT);
//     if (sock < 0)
//     {
//         return -1;
//     }

//     // Send HELLO message and receive response
//     std::string response = fetch(sock, tin::HELLO, "Hello Server");
//     std::cout << "Received response: " << response << std::endl;

//     // Send ECHO message and receive response
//     response = fetch(sock, tin::ECHO, "Echo this message");
//     std::cout << "Received response: " << response << std::endl;

//     // Send BYE message and receive response
//     response = fetch(sock, tin::BYE, "Goodbye Server");
//     std::cout << "Received response: " << response << std::endl;

//     // Close connection
//     closeConnection(sock);

//     return 0;
// }
#endif