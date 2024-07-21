#ifndef TCP_TRANSPORT_HPP
#define TCP_TRANSPORT_HPP

#include <Ipeer.hpp>
#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <functional>
#include <vector>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <mutex>
#include <queue>

namespace tin
{
    class TCPPeer : public IPeer
    {
    public:
        TCPPeer(int conn_fd, bool outbound)
            : conn_fd_(conn_fd), outbound_(outbound), active_(true) {}

        ~TCPPeer() override
        {
            closeStream();
        }

        ssize_t read(uint8_t *buffer, size_t length) override
        {
            return ::read(conn_fd_, buffer, length);
        }

        ssize_t write(const uint8_t *buffer, size_t length) override
        {
            return ::write(conn_fd_, buffer, length);
        }

        void close() override
        {
            if (active_)
            {
                ::close(conn_fd_);
                active_ = false;
            }
        }

        bool send(const std::vector<uint8_t> &data) override
        {
            return write(data.data(), data.size()) == static_cast<ssize_t>(data.size());
        }

        void closeStream() override
        {
            if (worker_ && worker_->joinable())
            {
                worker_->join();
            }
        }

        bool isOutbound() const
        {
            return outbound_;
        }

        bool isActive() const
        {
            return active_;
        }

    private:
        int conn_fd_;
        bool outbound_;
        std::atomic<bool> active_;
        std::shared_ptr<std::thread> worker_;
    };

    template <typename HandshakeFunc, typename Decoder, typename OnPeer>
    class TCPTransportOpts
    {
    public:
        std::string listenAddr;
        HandshakeFunc handshakeFunc;
        Decoder decoder;
        OnPeer onPeer;
    };

    template <typename HandshakeFunc, typename Decoder, typename OnPeer>
    class TCPTransport : public ITransport
    {
    public:
        TCPTransport(TCPTransportOpts<HandshakeFunc, Decoder, OnPeer> opts)
            : opts_(opts), listener_fd_(-1), rpcch_(1024) {}

        ~TCPTransport()
        {
            close();
        }

        std::string addr() const override
        {
            return opts_.listenAddr;
        }

        bool dial(const std::string &address) override
        {
            struct addrinfo hints, *res;
            std::memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;

            if (getaddrinfo(address.c_str(), "80", &hints, &res) != 0)
            {
                return false;
            }

            int conn_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
            if (conn_fd < 0)
            {
                freeaddrinfo(res);
                return false;
            }

            if (connect(conn_fd, res->ai_addr, res->ai_addrlen) < 0)
            {
                close(conn_fd);
                freeaddrinfo(res);
                return false;
            }

            freeaddrinfo(res);

            std::thread(&TCPTransport::handleConn, this, conn_fd, true).detach();

            return true;
        }

        bool listenAndAccept() override
        {
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(80);
            addr.sin_addr.s_addr = INADDR_ANY;

            listener_fd_ = socket(AF_INET, SOCK_STREAM, 0);
            if (listener_fd_ < 0)
            {
                return false;
            }

            if (bind(listener_fd_, (struct sockaddr *)&addr, sizeof(addr)) < 0)
            {
                close(listener_fd_);
                return false;
            }

            if (listen(listener_fd_, 10) < 0)
            {
                close(listener_fd_);
                return false;
            }

            std::thread(&TCPTransport::startAcceptLoop, this).detach();

            std::cout << "TCP transport listening on port: " << opts_.listenAddr << std::endl;

            return true;
        }

        std::shared_ptr<std::vector<uint8_t>> consume() override
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (rpcch_.empty())
            {
                return nullptr;
            }
            auto rpc = rpcch_.front();
            rpcch_.pop();
            return rpc;
        }

        bool close() override
        {
            if (listener_fd_ >= 0)
            {
                ::close(listener_fd_);
                listener_fd_ = -1;
                return true;
            }
            return false;
        }

    private:
        void startAcceptLoop()
        {
            while (true)
            {
                int conn_fd = accept(listener_fd_, nullptr, nullptr);
                if (conn_fd < 0)
                {
                    if (errno == EBADF || errno == ENOTSOCK)
                    {
                        return;
                    }
                    continue;
                }

                std::thread(&TCPTransport::handleConn, this, conn_fd, false).detach();
            }
        }

        void handleConn(int conn_fd, bool outbound)
        {
            auto peer = std::make_shared<TCPPeer>(conn_fd, outbound);

            if (!opts_.handshakeFunc(peer))
            {
                peer->close();
                return;
            }

            if (opts_.onPeer && !opts_.onPeer(peer))
            {
                peer->close();
                return;
            }

            uint8_t buffer[1024];
            while (peer->isActive())
            {
                ssize_t n = peer->read(buffer, sizeof(buffer));
                if (n <= 0)
                {
                    break;
                }

                auto rpc = std::make_shared<std::vector<uint8_t>>(buffer, buffer + n);
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    rpcch_.push(rpc);
                }
            }

            peer->close();
        }

        TCPTransportOpts<HandshakeFunc, Decoder, OnPeer> opts_;
        int listener_fd_;
        std::queue<std::shared_ptr<std::vector<uint8_t>>> rpcch_;
        std::mutex mutex_;
    };

}

#endif // TCP_TRANSPORT_HPP
