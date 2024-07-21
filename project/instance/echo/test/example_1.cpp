#include "../echo.hpp"
#include "../echoHandler.hpp"
#include "../../../server/server.hpp"

std::shared_ptr<tin::TCPServer> server;

void startServer()
{
    tin::EchoHandler echoHandler;
    server = std::make_shared<tin::TCPServer>(8080, &echoHandler, true);
    server->start();
}

void startClient()
{
    // Wait for server to start
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Echo echo;
    echo.send(R"(
    (127.0.0.1,8080)
    hello tin ;
    echo "hello everyone" ;
    goodbye ;
    )");

    echo.send(R"(
    (127.0.0.1,8080)
    hello job ;
    echo "hello tin" ;
    echo "how are you" ;
    goodbye ;
    )");

    echo.send(R"(
    (127.0.0.1,8080)
    hello tin ;
    echo "nice to meet you job!" ;
    echo "I am currently happy with this server ^_^ " ;
    goodbye ;
    )");

    echo.send(R"(
    (127.0.0.1,8080)
    hello hacker ;
    echo "===000000=000000===" ;
    echo "==000000000000000==" ;
    echo "===0000000000000===" ;
    echo "====00000000000====" ;
    echo "=====000000000=====" ;
    echo "======0000000======" ;
    echo "=======00000=======" ;
    echo "========000========" ;
    echo "=========0=========" ;
    goodbye ;
    )");

    echo.send(R"(
    (127.0.0.1,8080)
    hello job ;
    echo "what !!!!!!!" ;
    echo "-_-" ;
    goodbye ;
    )");
}

int main()
{
    std::thread serverThread(startServer);
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Give the server time to start

    std::thread clientThread(startClient);

    clientThread.join(); // Wait for the client to finish

    // Stop the server after the client has finished
    server->stop();
    serverThread.join();

    return 0;
}
