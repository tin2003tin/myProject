#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <unordered_map>
#include "global/security/SHA256/SHA256.hpp"

// Function to compute SHA-256 hash
std::string sha256(const std::string &data)
{
    tin::SHA256 sha;
    sha.update(data);
    auto digest = sha.digest();
    return tin::SHA256::toString(digest);
}

// Transaction class
class Transaction
{
public:
    std::string transactionID;
    std::string sender;
    std::string receiver;
    double amount;
    std::time_t timestamp;

    Transaction() = default;

    Transaction(std::string tid, std::string s, std::string r, double amt, std::time_t ts)
        : transactionID(tid), sender(s), receiver(r), amount(amt), timestamp(ts) {}

    std::string toString() const
    {
        std::stringstream ss;
        ss << transactionID << sender << receiver << amount << timestamp;
        return ss.str();
    }
};

// Merkle Tree class
class MerkleTree
{
public:
    MerkleTree(const std::vector<Transaction> &data)
    {
        buildTree(data);
    }

    std::string getRoot() const
    {
        return root;
    }

private:
    std::vector<std::string> leaves;
    std::vector<std::vector<std::string>> treeLevels;
    std::string root;

    void buildTree(const std::vector<Transaction> &data)
    {
        for (const auto &d : data)
        {
            leaves.push_back(sha256(d.toString()));
        }

        treeLevels.push_back(leaves);
        std::vector<std::string> currentLevel = leaves;
        while (currentLevel.size() > 1)
        {
            currentLevel = computeNextLevel(currentLevel);
            treeLevels.push_back(currentLevel);
        }

        root = currentLevel.front();
    }

    std::vector<std::string> computeNextLevel(const std::vector<std::string> &currentLevel)
    {
        std::vector<std::string> nextLevel;
        for (size_t i = 0; i < currentLevel.size(); i += 2)
        {
            if (i + 1 < currentLevel.size())
            {
                nextLevel.push_back(sha256(currentLevel[i] + currentLevel[i + 1]));
            }
            else
            {
                nextLevel.push_back(sha256(currentLevel[i] + currentLevel[i]));
            }
        }
        return nextLevel;
    }
};

// Block class with nonce and mining
class Block
{
public:
    std::string previousBlockHash;
    std::time_t timestamp;
    int nonce;
    std::string hash;
    MerkleTree merkleTree;
    std::vector<Transaction> transactions;

    Block(std::string previousHash, std::vector<Transaction> txs)
        : previousBlockHash(previousHash), transactions(txs), merkleTree(txs), timestamp(std::time(nullptr)), nonce(0)
    {
        hash = calculateHash();
    }

    std::string calculateHash() const
    {
        std::stringstream ss;
        ss << previousBlockHash << timestamp << nonce << merkleTree.getRoot();
        return sha256(ss.str());
    }

    void mineBlock(int difficulty)
    {
        std::string target(difficulty, '0');
        while (hash.substr(0, difficulty) != target)
        {
            nonce++;
            hash = calculateHash();
        }
        std::cout << "Block mined: " << hash << std::endl;
    }

    bool validateBlock() const
    {
        if (calculateHash() != hash)
            return false;
        if (merkleTree.getRoot() != MerkleTree(transactions).getRoot())
            return false;
        return true;
    }
};

// Blockchain class
class Blockchain
{
public:
    Blockchain()
    {
        std::vector<Transaction> genesisTransactions = {Transaction("0", "genesis", "genesis", 0, std::time(nullptr))};
        Block genesisBlock("0", genesisTransactions);
        chain.push_back(genesisBlock);
    }

    void addBlock(Block newBlock)
    {
        if (newBlock.validateBlock())
        {
            chain.push_back(newBlock);
        }
        else
        {
            std::cout << "Invalid block." << std::endl;
        }
    }

    Block getLastBlock() const
    {
        return chain.back();
    }

    void printBlockchain() const
    {
        std::cout << "Blockchain:" << std::endl;
        for (const auto &block : chain)
        {
            std::cout << "Block Hash: " << block.hash << std::endl;
            std::cout << "Previous Block Hash: " << block.previousBlockHash << std::endl;
            std::cout << "Merkle Root: " << block.merkleTree.getRoot() << std::endl;
            std::cout << "Nonce: " << block.nonce << std::endl;
            std::cout << "Transactions: " << std::endl;
            for (const auto &tx : block.transactions)
            {
                std::cout << "  " << tx.toString() << std::endl;
            }
        }
    }

private:
    std::vector<Block> chain;
};

// Node class
class Node
{
public:
    Node(Blockchain &blockchain, const std::string &nodeName) : blockchain(blockchain), name(nodeName) {}

    void createTransaction(const std::string &sender, const std::string &receiver, double amount)
    {
        Transaction transaction(std::to_string(transactionCounter++), sender, receiver, amount, std::time(nullptr));
        pendingTransactions.push_back(transaction);
    }

    void minePendingTransactions(int difficulty)
    {
        if (pendingTransactions.empty())
        {
            std::cout << "No transactions to mine." << std::endl;
            return;
        }

        Block newBlock(blockchain.getLastBlock().hash, pendingTransactions);
        newBlock.mineBlock(difficulty);
        blockchain.addBlock(newBlock);
        // broadcastBlock(newBlock);

        pendingTransactions.clear();
    }

    void receiveBlock(const Block &newBlock)
    {
        if (newBlock.validateBlock() && newBlock.previousBlockHash == blockchain.getLastBlock().hash)
        {
            blockchain.addBlock(newBlock);
            std::cout << name << " added the block to the blockchain." << std::endl;
        }
        else
        {
            std::cout << name << " received an invalid block." << std::endl;
        }
    }

    void broadcastBlock(const Block &newBlock)
    {
        std::cout << name << " is broadcasting the block..." << std::endl;
        for (const auto &node : networkNodes)
        {
            if (node != this)
            {
                node->receiveBlock(newBlock);
            }
        }
    }

    static void addNetworkNode(Node *node)
    {
        networkNodes.push_back(node);
    }

private:
    Blockchain &blockchain;
    std::vector<Transaction> pendingTransactions;
    int transactionCounter = 0;
    std::string name;
    static std::vector<Node *> networkNodes;
};

std::vector<Node *> Node::networkNodes;

// Main function
int main()
{
    Blockchain blockchain;

    Node node1(blockchain, "Node1");
    Node node2(blockchain, "Node2");
    Node node3(blockchain, "Node3");

    Node::addNetworkNode(&node1);
    Node::addNetworkNode(&node2);
    // Node::addNetworkNode(&node3);

    node1.createTransaction("Alice", "Bob", 50);
    node1.createTransaction("Bob", "Charlie", 25);

    node2.createTransaction("Charlie", "Dave", 10);
    node2.createTransaction("Dave", "Eve", 5);

    node3.createTransaction("Tin", "Job", 10);
    node3.createTransaction("Pun", "Nut", 5);

    std::cout << "Node1 is mining..." << std::endl;
    node1.minePendingTransactions(4);

    // std::cout << "Node2 is mining..." << std::endl;
    // node2.minePendingTransactions(4);

    // std::cout << "Node3 is mining..." << std::endl;
    // node3.minePendingTransactions(4);
    // std::cout << "\n\n\n\n\n\n";

    blockchain.printBlockchain();

    return 0;
}
