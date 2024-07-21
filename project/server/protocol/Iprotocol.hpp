#ifndef IPROTOCOL_HPP
#define IPROTOCOL_HPP

#include <string>

namespace tin
{

    class IProtocol
    {
    public:
        virtual ~IProtocol() = default;
        virtual std::string serialize() const = 0;
        virtual uint16_t get_type() const = 0;
        virtual std::string get_body() const = 0;

        static IProtocol *deserialize(const char *data, size_t size);
    };

}

#endif 
