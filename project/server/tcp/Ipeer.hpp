#ifndef TRANSPORT_HPP
#define TRANSPORT_HPP

#include <string>
#include <vector>
#include <memory>

namespace tin
{
    class IConn
    {
    public:
        virtual ~IConn() = default;
        virtual ssize_t read(uint8_t *buffer, size_t length) = 0;
        virtual ssize_t write(const uint8_t *buffer, size_t length) = 0;
        virtual void close() = 0;

    };

    class IPeer : public IConn
    {
    public:
        virtual ~IPeer() = default;
        virtual bool send(const std::vector<uint8_t> &data) = 0;
        virtual void closeStream() = 0;
    };
    class ITransport
    {
    public:
        virtual ~ITransport() = default;
        virtual std::string addr() const = 0;
        virtual bool dial(const std::string &address) = 0;
        virtual std::shared_ptr<std::vector<uint8_t>> consume() = 0;
        virtual bool close() = 0;
    };
}

#endif
