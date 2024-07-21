#ifndef BLOCKCHAIN_BLOCKHEADER
#define BLOCKCHAIN_BLOCKHEADER

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include "sha.hpp"

namespace tin_blockchain
{
    class BlockHeader
    {
    public:
        std::string hash;
        std::string previousHash;
        std::string merkleRoot;
        uint64_t timestamp;
        int difficulty;
        uint64_t nonce;

        BlockHeader(const std::string &previousHash, const std::string &merkleRoot, uint64_t timestamp, int difficulty)
            : previousHash(previousHash), merkleRoot(merkleRoot), timestamp(timestamp), difficulty(difficulty), nonce(0)
        {
        }

        void setHash(std::string hash)
        {
            this->hash = hash;
        }

        std::string serialize() const
        {
            std::ostringstream oss;
            oss << previousHash << merkleRoot << timestamp << difficulty << nonce;
            return oss.str();
        }

        std::string computeHash() const
        {
            return sha256(sha256(serialize()));
        }

        std::string toString() const
        {
            std::ostringstream oss;
            oss << "{"
                << " \"hash\": \"" << hash << "\""
                << ", \"previousHash\": \"" << previousHash << "\""
                << ", \"merkleRoot\": \"" << merkleRoot << "\""
                << ", \"timestamp\": " << timestamp
                << ", \"difficulty\": " << difficulty
                << ", \"nonce\": " << nonce
                << "}";
            return oss.str();
        }
    };

}

#endif
