#ifndef BLOCKCHAIN_VOUT
#define BLOCKCHAIN_VOUT

#include <string>
#include <sstream>
#include <iomanip>

/**
 * @todo complete all variable in Output
 *  (int type; bool spent; int n; std::string script; std::vector<std::pair<uint64_t, int>> spending_outpoints;)
 */
namespace tin_blockchain
{
    class Output
    {
    public:
        double value;
        uint64_t txIndex;
        std::string addr;

        Output(double value, uint64_t txIndex, const std::string &addr)
            : value(value), txIndex(txIndex), addr(addr) {}

        std::string toString() const
        {
            std::ostringstream oss;
            oss << "{"
                << "\"value\": " << value
                << ", \"txIndex\": " << txIndex
                << ", \"addr\": \"" << addr << "\""
                << "}";
            return oss.str();
        }

        std::string serialize() const
        {
            std::ostringstream oss;
            oss << std::hex << std::setw(16) << std::setfill('0') << (uint64_t)(value * 1000000000) << addr;
            return oss.str();
        }
    };
}

#endif
