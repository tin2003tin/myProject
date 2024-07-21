#include "handler/Ihandler.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <atomic>
#include <chrono>

#define PORT 8080
#define SERVER_TIMEOUT 10 // Server will run for 5 seconds for testing

namespace tin
{
    class TCPServer
    {
    public:
        TCPServer(int port, tin::IHandler *handler, bool testMode = false)
            : port(port), handler(handler), server_fd(0), running(true), testMode(testMode) {}

        bool start()
        {
            if (!create_socket() || !bind_socket() || !listen_socket())
            {
                return false;
            }

            std::cout << "Server listening on port " << port << std::endl;

            auto start_time = std::chrono::steady_clock::now();

            while (running.load())
            {
                int client_socket;
                struct sockaddr_in address;
                socklen_t addrlen = sizeof(address);

                if ((client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0)
                {
                    if (!running.load())
                    {
                        // Exit loop if the server is stopping
                        break;
                    }
                    std::cerr << "Accept failed" << std::endl;
                    continue;
                }

                std::cout << "Connection accepted" << std::endl;
                std::thread(&IHandler::handle, handler, client_socket).detach();

                // Check if the server has been running for too long
                if (testMode)
                {
                    printf("test");
                    auto current_time = std::chrono::steady_clock::now();
                    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
                    if (elapsed >= SERVER_TIMEOUT)
                    {
                        stop();
                    }
                }
            }

            // Clean up resources
            close(server_fd);
            std::cout << "Server stopped" << std::endl;
            return true;
        }

        void stop()
        {
            running.store(false);
            // Close the server socket to interrupt accept
            if (server_fd != 0)
            {
                close(server_fd);
            }
        }

    private:
        int port;
        int server_fd;
        IHandler *handler;
        std::atomic<bool> running;
        bool testMode;

        bool create_socket()
        {
            if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
            {
                std::cerr << "Socket creation failed" << std::endl;
                return false;
            }

            int opt = 1;
            if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
            {
                std::cerr << "Setsockopt failed" << std::endl;
                close(server_fd);
                return false;
            }

            return true;
        }

        bool bind_socket()
        {
            struct sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(port);

            if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
            {
                std::cerr << "Bind failed" << std::endl;
                close(server_fd);
                return false;
            }

            return true;
        }

        bool listen_socket()
        {
            if (listen(server_fd, 3) < 0)
            {
                std::cerr << "Listen failed" << std::endl;
                close(server_fd);
                return false;
            }

            return true;
        }
    };
}
