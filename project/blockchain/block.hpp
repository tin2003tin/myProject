#ifndef BLOCKCHAIN_BLOCK
#define BLOCKCHAIN_BLOCK

#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <fstream>
#include "transaction.hpp"
#include "blockheader.hpp"
#include "merkleTree.hpp"

namespace tin_blockchain
{
    class Block
    {
    public:
        BlockHeader header;
        std::vector<Transaction> transactions;

        Block(const BlockHeader &header, const std::vector<Transaction> &transactions)
            : header(header), transactions(transactions) {}

        std::string toString() const
        {
            std::ostringstream oss;
            oss << "{"
                << "\"header\": " << header.toString()
                << ", \"transactions\": [";
            for (size_t i = 0; i < transactions.size(); ++i)
            {
                oss << transactions[i].toString();
                if (i < transactions.size() - 1)
                    oss << ", ";
            }
            oss << "]}";
            return oss.str();
        }

        void printBlock() const
        {
            std::cout << toString() << std::endl;
        }

        void writeToJsonFile(const std::string &filename) const
        {
            std::ofstream file(filename);
            if (file.is_open())
            {
                file << toString();
                file.close();
                std::cout << "Block written to " << filename << std::endl;
            }
            else
            {
                std::cerr << "Unable to open file " << filename << std::endl;
            }
        }
        std::string mine()
        {
            std::string target(header.difficulty, '0');
            while (header.computeHash().substr(0, header.difficulty) != target)
            {
                header.nonce++;
            }
            return header.computeHash();
        }
    };
}

#endif
