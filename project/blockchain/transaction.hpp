#ifndef BLOCKCHAIN_TRANSACTION
#define BLOCKCHAIN_TRANSACTION

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

#include "input.hpp"
#include "output.hpp"
#include "sha.hpp"

/**
 * @todo complete all variable in Transaction
 *  (int ver; int vin_sz; int vout_sz; int size; int weight; std::string relayed_by; unsigned int lock_time;)
 *  (bool double_spend;)
 */
namespace tin_blockchain
{
    class Transaction
    {
    public:
        std::string hash;
        int fee;
        uint64_t txIndex;
        unsigned int time;
        int blockIndex;
        int blockHeight;
        std::vector<Input> inputs;
        std::vector<Output> out;

        Transaction(int fee, uint64_t txIndex, unsigned int time, int blockIndex,
                    int blockHeight, const std::vector<Input> &inputs, const std::vector<Output> &out)
            : fee(fee), txIndex(txIndex), time(time), blockIndex(blockIndex),
              blockHeight(blockHeight), inputs(inputs), out(out)
        {
            createHash();
        }

        void createHash()
        {
            std::string serializedData = serialize();
            hash = sha256(sha256(serializedData));
        }

        std::string toString() const
        {
            std::ostringstream oss;
            oss << "{"
                << "\"hash\": " << std::quoted(hash)
                << ", \"fee\": " << fee
                << ", \"time\": " << time
                << ", \"blockIndex\": " << blockIndex
                << ", \"blockHeight\": " << blockHeight
                << ", \"inputs\": [";
            for (size_t i = 0; i < inputs.size(); ++i)
            {
                oss << inputs[i].toString();
                if (i < inputs.size() - 1)
                    oss << ", ";
            }
            oss << "], \"out\": [";
            for (size_t i = 0; i < out.size(); ++i)
            {
                oss << out[i].toString();
                if (i < out.size() - 1)
                    oss << ", ";
            }
            oss << "]}";
            return oss.str();
        }

        std::string serialize() const
        {
            std::ostringstream oss;
            oss << fee << txIndex << time << blockIndex << blockHeight;
            for (const auto &in : inputs)
            {
                oss << in.serialize();
            }
            for (const auto &ou : out)
            {
                oss << ou.serialize();
            }
            return oss.str();
        }
    };
}

#endif
