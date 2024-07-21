#ifndef BLOCKCHAIN_VIN
#define BLOCKCHAIN_VIN

#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

/**
 * @todo complete all variable in PrevOut
 *  (bool spent; int type; std::string script; std::vector<std::pair<uint64_t, int>> spending_outpoints;)
 * @todo complete all variable Input
 *  (unsigned int sequence;  std::string witness; std::string script; int index;)
 */
namespace tin_blockchain
{
    class PrevOut
    {
    public:
        double value;
        uint64_t txIndex;
        std::string addr;

        PrevOut(double value, uint64_t txIndex, const std::string &addr)
            : value(value), txIndex(txIndex), addr(addr) {}

        std::string toString() const
        {
            std::ostringstream oss;
            oss << "{"
                << ", \"value\": " << value
                << ", \"txIndex\": " << txIndex
                << ", \"addr\": " << std::quoted(addr) << "}";
            return oss.str();
        }

        std::string serialize() const
        {
            std::ostringstream oss;
            oss << std::hex << txIndex << std::setw(8) << std::setfill('0');
            return oss.str();
        }
    };

    class Input
    {
    public:
        PrevOut prev_out;

        Input(const PrevOut &prev_out)
            : prev_out(prev_out) {}

        std::string toString() const
        {
            std::ostringstream oss;
            oss << "{"
                << "\"prev_out\": " << prev_out.toString()
                << "}";
            return oss.str();
        }

        std::string serialize() const
        {
            std::ostringstream oss;
            oss << prev_out.serialize();
            return oss.str();
        }
    };
}

#endif
