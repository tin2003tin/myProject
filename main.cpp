#include "project/instance/echo/echo.hpp"

int main()
{
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