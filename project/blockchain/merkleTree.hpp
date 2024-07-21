#ifndef BLOCKCHAIN_MERKLE_TREE
#define BLOCKCHAIN_MERKLE_TREE

#include <vector>
#include <cstdint>
#include <string>

#include "sha.hpp"
#include "transaction.hpp"

namespace tin_blockchain
{

    class MerkleTree
    {
    public:
        static std::string computeMerkleRoot(const std::vector<Transaction> &transactions)
        {
            if (transactions.empty())
                return "";

            std::vector<std::string> merkleLeaves;
            for (const auto &tx : transactions)
            {
                merkleLeaves.push_back(tx.hash);
            }

            while (merkleLeaves.size() > 1)
            {
                if (merkleLeaves.size() % 2 != 0)
                {
                    merkleLeaves.push_back(merkleLeaves.back());
                }

                std::vector<std::string> newLevel;
                for (size_t i = 0; i < merkleLeaves.size(); i += 2)
                {
                    newLevel.push_back(sha256(merkleLeaves[i] + merkleLeaves[i + 1]));
                }
                merkleLeaves = newLevel;
            }

            return merkleLeaves[0];
        }
    };
}

#endif
