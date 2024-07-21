#ifndef ICLIENTHANDLER_HPP
#define ICLIENTHANDLER_HPP

namespace tin
{

    class IHandler
    {
    public:
        virtual ~IHandler() = default;
        virtual void handle(int client_socket) = 0;
    };

} // namespace tin

#endif // ICLIENTHANDLER_HPP
