#include "../../server/server.hpp"
#include "echoHandler.hpp"

int main()
{
    tin::EchoHandler echoHandler;
    tin::TCPServer server(8080, &echoHandler);
    server.start();
}