#ifndef ECHO_COMPILER_HPP
#define ECHO_COMPILER_HPP

#include "../../compiler/LRcomplier.hpp"
#include "echoProtocol.hpp"
#include "echoHandler.hpp"
#include "echoCon.hpp"
#include <stack>

std::string grammarRule = R"(
        S' -> #Request
        #Request -> #Destination #Function              ## Handler::execute
        #Destination -> ( #Address , #Port )
        #Address -> #ID . #ID . #ID . #ID               ## Handler::setAddress
        #Port -> #ID                                    ## Handler::setPort
        #Function -> hello #ID ; #Function                  ## Handler::hello
        #Function -> echo #ID ; #Function               ## Handler::echo
        #Function -> goodbye ; #Function                ## Handler::goodbye
        #Function -> ε
    )";

HANDLER(Handler)
{
    class Item
    {
    public:
        tin::MessageType type;
        std::string message;
        Item(tin::MessageType type, std::string message) : type(type), message(message) {}
    };

    std::stack<Item> stack;
    int port;
    std::string server_address;

    DECLARE(
        REGISTER(Handler::hello),
        REGISTER(Handler::echo),
        REGISTER(Handler::goodbye),
        REGISTER(Handler::setAddress),
        REGISTER(Handler::setPort),
        REGISTER(Handler::execute), );

    FUNC(hello)
    {
        // std::cout << "HELLO" << std::endl;
        stack.push(Item(tin::HELLO, input[0]));
    }

    FUNC(echo)
    {
        // std::cout << "ECHO: " << input[0] << std::endl;
        stack.push(Item(tin::ECHO, input[0]));
    }

    FUNC(goodbye)
    {
        // std::cout << "GOODBYE" << std::endl;
        stack.push(Item(tin::BYE, ""));
    }
    FUNC(setAddress)
    {
        std::reverse(input.begin(), input.end());
        std::string address = input[0] + "." + input[1] + "." + input[2] + "." + input[3];
        // std::cout << "server address: " << address << std::endl;
        server_address = address;
    }
    FUNC(setPort)
    {
        // std::cout << "port: " << input[0] << std::endl;
        port = std::stoi(input[0]);
    }
    FUNC(execute)
    {

        Conn conn;
        int sock = conn.createConnection(server_address, port);
        if (sock < 0)
        {
            return;
        }
        std::cout << "try to execute..." << std::endl;
        while (!stack.empty())
        {
            Item top = stack.top();
            stack.pop();
            std::string response = conn.fetch(sock, top.type, top.message);
            std::cout << "Received response: " << response << std::endl;
        }
        conn.closeConnection(sock);
        std::cout << "Finished!!!" << std::endl;
    }
};

#endif