#ifndef ECHO_PROTOCOL_HPP
#define ECHO_PROTOCOL_HPP

#include "../../server/protocol/Iprotocol.hpp"
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <string>
#include <netinet/in.h>
#include <stdexcept>

namespace tin
{

    enum MessageType
    {
        HELLO = 1,
        ECHO,
        BYE,
        UNKNOWN
    };

    struct ProtocolHeader
    {
        uint32_t length;
        uint16_t type;
        uint16_t checksum;
    };

    class ProtocolMessage : public tin::IProtocol
    {
    public:
        ProtocolMessage(uint16_t type, const std::string &body)
            : type(type), body(body)
        {
            length = sizeof(ProtocolHeader) + body.size();
            checksum = calculate_checksum();
        }

        std::string serialize() const override
        {
            std::string message;
            ProtocolHeader header = {htonl(length), htons(type), htons(checksum)};
            message.append(reinterpret_cast<const char *>(&header), sizeof(header));
            message.append(body);
            return message;
        }

        static IProtocol *deserialize(const char *data, size_t size)
        {
            if (size < sizeof(ProtocolHeader))
            {
                throw std::runtime_error("Invalid message size");
            }

            ProtocolHeader header;
            std::memcpy(&header, data, sizeof(header));

            header.length = ntohl(header.length);
            header.type = ntohs(header.type);
            header.checksum = ntohs(header.checksum);

            if (header.length != size)
            {
                throw std::runtime_error("Invalid message length");
            }

            std::string body(data + sizeof(ProtocolHeader), size - sizeof(ProtocolHeader));

            return new ProtocolMessage(header.type, body);
        }

        uint16_t get_type() const override
        {
            return type;
        }

        std::string get_body() const override
        {
            return body;
        }

    private:
        uint32_t length;
        uint16_t type;
        uint16_t checksum;
        std::string body;

        uint16_t calculate_checksum() const
        {
            uint16_t sum = 0;
            for (char c : body)
            {
                sum += c;
            }
            return sum;
        }
    };

}

#endif