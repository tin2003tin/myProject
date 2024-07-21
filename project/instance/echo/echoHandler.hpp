#ifndef ECHO_HANDLER_HPP
#define ECHO_HANDLER_HPP

#include "../../server/protocol/Iprotocol.hpp"
#include "../../server/handler/Ihandler.hpp"
#include "echoProtocol.hpp"
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <string>
#include <netinet/in.h>
#include <stdexcept>

#define BUFFER_SIZE 1024

namespace tin
{

    class EchoHandler : public IHandler
    {
    public:
        void handle(int client_socket) override
        {
            char buffer[BUFFER_SIZE] = {0};
            int read_val;

            while (true)
            {
                read_val = read(client_socket, buffer, BUFFER_SIZE);
                if (read_val <= 0)
                {
                    std::cerr << "Read failed or client disconnected" << std::endl;
                    break;
                }

                try
                {
                    tin::IProtocol *message = tin::ProtocolMessage::deserialize(buffer, read_val);
                    std::string response;

                    if (message->get_type() == HELLO)
                    {
                        std::cout << "to: " << message->get_body() << std::endl;
                        response = ProtocolMessage(HELLO, "WELCOME").serialize();
                    }
                    else if (message->get_type() == ECHO)
                    {
                        std::cout << "- " << message->get_body() << std::endl;
                        response = ProtocolMessage(ECHO, "You have sent: " + message->get_body()).serialize();
                    }
                    else if (message->get_type() == BYE)
                    {
                        response = ProtocolMessage(BYE, "GOODBYE").serialize();
                        write(client_socket, response.c_str(), response.size());
                        delete message;
                        break;
                    }
                    else
                    {
                        response = ProtocolMessage(UNKNOWN, "UNKNOWN COMMAND").serialize();
                    }

                    write(client_socket, response.c_str(), response.size());
                    delete message;
                }
                catch (const std::runtime_error &e)
                {
                    std::cerr << "Failed to deserialize message: " << e.what() << std::endl;
                }
            }

            close(client_socket);
        }
    };

} // namespace tin

#endif
