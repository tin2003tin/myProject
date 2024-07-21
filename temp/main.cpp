#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <unordered_set>
#include <memory>
#include <mutex>
#include <thread>
#include <boost/asio.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

using boost::asio::ip::tcp;
using namespace std::placeholders;

// Utility function to generate a SHA256 hash
std::string computeHash(const std::string &data)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)data.c_str(), data.size(), hash);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
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
    std::string signature;

    Transaction() = default;

    Transaction(std::string tid, std::string s, std::string r, double amt)
        : transactionID(tid), sender(s), receiver(r), amount(amt)
    {
        timestamp = std::time(nullptr);
    }

    std::string toString() const
    {
        std::stringstream ss;
        ss << transactionID << sender << receiver << amount << timestamp;
        return ss.str();
    }

    void signTransaction(RSA *privateKey)
    {
        std::string data = toString();
        unsigned char *sig = new unsigned char[RSA_size(privateKey)];
        unsigned int sigLen;
        if (RSA_sign(NID_sha256, (unsigned char *)data.c_str(), data.size(), sig, &sigLen, privateKey) == 1)
        {
            signature = std::string(reinterpret_cast<char *>(sig), sigLen);
        }
        delete[] sig;
    }

    bool verifyTransaction(RSA *publicKey) const
    {
        std::string data = toString();
        return RSA_verify(NID_sha256, (unsigned char *)data.c_str(), data.size(), (unsigned char *)signature.c_str(), signature.size(), publicKey) == 1;
    }

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & transactionID;
        ar & sender;
        ar & receiver;
        ar & amount;
        ar & timestamp;
        ar & signature;
    }
};

// Node class for Merkle tree
class Node
{
public:
    std::string hash;
    Node *left;
    Node *right;

    Node(std::string hash, Node *left = nullptr, Node *right = nullptr)
    {
        this->hash = hash;
        this->left = left;
        this->right = right;
    }
};

// MerkleTree class
class MerkleTree
{
public:
    MerkleTree(const std::vector<Transaction> &transactions)
    {
        buildTree(transactions);
    }

    Node *getRoot()
    {
        return root;
    }

private:
    Node *root;

    void buildTree(const std::vector<Transaction> &transactions)
    {
        std::vector<Node *> nodes;

        for (const auto &tx : transactions)
        {
            nodes.push_back(new Node(computeHash(tx.toString())));
        }

        while (nodes.size() > 1)
        {
            if (nodes.size() % 2 != 0)
            {
                nodes.push_back(nodes.back()); // Duplicate last node if odd number of nodes
            }

            std::vector<Node *> newLevel;
            for (size_t i = 0; i < nodes.size(); i += 2)
            {
                newLevel.push_back(new Node(computeHash(nodes[i]->hash + nodes[i + 1]->hash), nodes[i], nodes[i + 1])));
            }
            nodes = newLevel;
        }

        root = nodes[0];
    }
};

// Block class
class Block
{
public:
    std::string previousHash;
    std::vector<Transaction> transactions;
    std::time_t timestamp;
    int nonce;
    std::string hash;

    Block() = default;

    Block(std::string prevHash, const std::vector<Transaction> &txs)
        : previousHash(prevHash), transactions(txs), nonce(0)
    {
        timestamp = std::time(nullptr);
        hash = computeHash();
    }

    std::string computeHash() const
    {
        std::stringstream ss;
        ss << previousHash << timestamp << nonce;
        for (const auto &tx : transactions)
        {
            ss << tx.toString();
        }
        return ::computeHash(ss.str());
    }

    void mineBlock(int difficulty)
    {
        std::string target(difficulty, '0');
        while (hash.substr(0, difficulty) != target)
        {
            nonce++;
            hash = computeHash();
        }
        std::cout << "Block mined: " << hash << std::endl;
    }

    bool hasValidTransactions(RSA *publicKey) const
    {
        for (const auto &tx : transactions)
        {
            if (!tx.verifyTransaction(publicKey))
            {
                return false;
            }
        }
        return true;
    }

    bool isValidPoW(int difficulty) const
    {
        std::string target(difficulty, '0');
        return hash.substr(0, difficulty) == target && hash == computeHash();
    }

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & previousHash;
        ar & transactions;
        ar & timestamp;
        ar & nonce;
        ar & hash;
    }
};

// Blockchain class
class Blockchain
{
public:
    Blockchain()
    {
        chain.push_back(createGenesisBlock());
        difficulty = 4;
    }

    void addBlock(Block newBlock)
    {
        newBlock.previousHash = getLatestBlock().hash;
        newBlock.mineBlock(difficulty);
        chain.push_back(newBlock);
    }

    bool isChainValid(RSA *publicKey) const
    {
        for (size_t i = 1; i < chain.size(); ++i)
        {
            Block currentBlock = chain[i];
            Block previousBlock = chain[i - 1];

            if (currentBlock.hash != currentBlock.computeHash())
            {
                return false;
            }

            if (currentBlock.previousHash != previousBlock.hash)
            {
                return false;
            }

            if (!currentBlock.hasValidTransactions(publicKey))
            {
                return false;
            }

            if (!currentBlock.isValidPoW(difficulty))
            {
                return false;
            }
        }
        return true;
    }

    Block getLatestBlock() const
    {
        return chain.back();
    }

    const std::vector<Block> &getChain() const
    {
        return chain;
    }

    void replaceChain(const std::vector<Block> &newChain)
    {
        if (newChain.size() > chain.size() && isChainValid(publicKey))
        {
            chain = newChain;
        }
    }

private:
    std::vector<Block> chain;
    int difficulty;

    Block createGenesisBlock()
    {
        std::vector<Transaction> txs = {Transaction("0", "Genesis", "Genesis", 0)};
        Block genesisBlock("0", txs);
        genesisBlock.mineBlock(difficulty);
        return genesisBlock;
    }

    RSA *publicKey;
};

// Peer-to-peer Node class
class P2PNode : public std::enable_shared_from_this<P2PNode>
{
public:
    P2PNode(boost::asio::io_context &io_context, const std::string &id, RSA *privKey, RSA *pubKey, const tcp::endpoint &endpoint)
        : nodeID(id), privateKey(privKey), publicKey(pubKey), acceptor(io_context, endpoint), socket(io_context), strand(io_context)
    {
        blockchain = std::make_unique<Blockchain>();
        do_accept();
    }

    void addBootstrapPeers(const std::vector<std::pair<std::string, std::string>> &bootstrapPeers)
    {
        for (const auto &peer : bootstrapPeers)
        {
            addPeer(peer.first, peer.second);
        }
    }

    void broadcastTransaction(const Transaction &transaction)
    {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto &peer : peers)
        {
            peer->sendTransaction(transaction);
        }
    }

    void receiveTransaction(const Transaction &transaction)
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (transaction.verifyTransaction(publicKey))
        {
            pendingTransactions.push_back(transaction);
            std::cout << "Transaction " << transaction.transactionID << " received by node " << nodeID << std::endl;
        }
    }

    void createBlock()
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (pendingTransactions.empty())
            return;

        Block newBlock(blockchain->getLatestBlock().hash, pendingTransactions);
        blockchain->addBlock(newBlock);
        pendingTransactions.clear();
        broadcastBlock(newBlock);
    }

    void broadcastBlock(const Block &block)
    {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto &peer : peers)
        {
            peer->sendBlock(block);
        }
    }

    void receiveBlock(const Block &block)
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (block.isValidPoW(blockchain->getDifficulty()) && block.hasValidTransactions(publicKey))
        {
            blockchain->addBlock(block);
            std::cout << "Block received by node " << nodeID << std::endl;
        }
        else
        {
            requestBlockchain();
        }
    }

    void requestBlockchain()
    {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto &peer : peers)
        {
            peer->sendBlockchain(blockchain->getChain());
        }
    }

    void receiveBlockchain(const std::vector<Block> &newChain)
    {
        std::lock_guard<std::mutex> lock(mutex);
        blockchain->replaceChain(newChain);
    }

    bool validateBlockchain()
    {
        std::lock_guard<std::mutex> lock(mutex);
        return blockchain->isChainValid(publicKey);
    }

private:
    std::string nodeID;
    RSA *privateKey;
    RSA *publicKey;
    std::unique_ptr<Blockchain> blockchain;
    std::vector<Transaction> pendingTransactions;
    std::unordered_set<std::shared_ptr<P2PNode>> peers;
    tcp::acceptor acceptor;
    tcp::socket socket;
    boost::asio::strand<boost::asio::io_context::executor_type> strand;
    std::mutex mutex;

    void do_accept()
    {
        acceptor.async_accept(
            boost::asio::make_strand(strand.context()),
            [this](boost::system::error_code ec, tcp::socket peer)
            {
                if (!ec)
                {
                    std::cout << "New peer connected." << std::endl;
                    auto new_peer = std::make_shared<P2PNode>(std::move(peer), nodeID, privateKey, publicKey, acceptor.local_endpoint());
                    peers.insert(new_peer);
                    new_peer->start();
                    do_accept();
                }
            });
    }

    void addPeer(const std::string &host, const std::string &port)
    {
        tcp::resolver resolver(socket.get_executor().context());
        auto endpoints = resolver.resolve(host, port);
        boost::asio::async_connect(socket, endpoints,
                                   [this](boost::system::error_code ec, tcp::endpoint)
                                   {
                                       if (!ec)
                                       {
                                           std::cout << "Connected to peer." << std::endl;
                                           start();
                                       }
                                   });
    }

    void start()
    {
        do_read();
    }

    void do_read()
    {
        boost::asio::async_read(socket, boost::asio::buffer(data, max_length),
                                boost::asio::bind_executor(strand,
                                                           [this](boost::system::error_code ec, std::size_t length)
                                                           {
                                                               if (!ec)
                                                               {
                                                                   std::cout << "Message received: " << std::string(data, length) << std::endl;
                                                                   do_read();
                                                               }
                                                           }));
    }

    void sendTransaction(const Transaction &transaction)
    {
        std::ostringstream archive_stream;
        boost::archive::text_oarchive archive(archive_stream);
        archive << transaction;
        std::string outbound_data = archive_stream.str();

        boost::asio::async_write(socket, boost::asio::buffer(outbound_data),
                                 boost::asio::bind_executor(strand,
                                                            [this](boost::system::error_code ec, std::size_t /*length*/)
                                                            {
                                                                if (!ec)
                                                                {
                                                                    std::cout << "Transaction sent to peer." << std::endl;
                                                                }
                                                            }));
    }

    void sendBlock(const Block &block)
    {
        std::ostringstream archive_stream;
        boost::archive::text_oarchive archive(archive_stream);
        archive << block;
        std::string outbound_data = archive_stream.str();

        boost::asio::async_write(socket, boost::asio::buffer(outbound_data),
                                 boost::asio::bind_executor(strand,
                                                            [this](boost::system::error_code ec, std::size_t /*length*/)
                                                            {
                                                                if (!ec)
                                                                {
                                                                    std::cout << "Block sent to peer." << std::endl;
                                                                }
                                                            }));
    }

    void sendBlockchain(const std::vector<Block> &chain)
    {
        std::ostringstream archive_stream;
        boost::archive::text_oarchive archive(archive_stream);
        archive << chain;
        std::string outbound_data = archive_stream.str();

        boost::asio::async_write(socket, boost::asio::buffer(outbound_data),
                                 boost::asio::bind_executor(strand,
                                                            [this](boost::system::error_code ec, std::size_t /*length*/)
                                                            {
                                                                if (!ec)
                                                                {
                                                                    std::cout << "Blockchain sent to peer." << std::endl;
                                                                }
                                                            }));
    }

    enum
    {
        max_length = 1024
    };
    char data[max_length];
};

// Utility function to generate RSA key pair
void generateRSAKeyPair(RSA **privateKey, RSA **publicKey)
{
    RSA *keyPair = RSA_generate_key(2048, RSA_F4, nullptr, nullptr);

    BIO *privateBIO = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPrivateKey(privateBIO, keyPair, nullptr, nullptr, 0, nullptr, nullptr);
    *privateKey = PEM_read_bio_RSAPrivateKey(privateBIO, nullptr, nullptr, nullptr);
    BIO_free_all(privateBIO);

    BIO *publicBIO = BIO_new(BIO_s_mem());
    PEM_write_bio_RSA_PUBKEY(publicBIO, keyPair);
    *publicKey = PEM_read_bio_RSA_PUBKEY(publicBIO, nullptr, nullptr, nullptr);
    BIO_free_all(publicBIO);

    RSA_free(keyPair);
}

int main()
{
    try
    {
        boost::asio::io_context io_context;

        RSA *privateKey1;
        RSA *publicKey1;
        generateRSAKeyPair(&privateKey1, &publicKey1);

        RSA *privateKey2;
        RSA *publicKey2;
        generateRSAKeyPair(&privateKey2, &publicKey2);

        tcp::endpoint endpoint1(tcp::v4(), 1234);
        auto node1 = std::make_shared<P2PNode>(io_context, "Node1", privateKey1, publicKey1, endpoint1);

        tcp::endpoint endpoint2(tcp::v4(), 1235);
        auto node2 = std::make_shared<P2PNode>(io_context, "Node2", privateKey2, publicKey2, endpoint2);

        node1->addBootstrapPeers({{"localhost", "1235"}});

        std::thread t([&io_context]()
                      { io_context.run(); });

        Transaction tx1("tx1", "Alice", "Bob", 50.0);
        tx1.signTransaction(privateKey1);

        node1->broadcastTransaction(tx1);

        node1->createBlock();
        node2->createBlock();

        std::cout << "Blockchain valid at node1: " << node1->validateBlockchain() << std::endl;
        std::cout << "Blockchain valid at node2: " << node2->validateBlockchain() << std::endl;

        t.join();

        RSA_free(privateKey1);
        RSA_free(publicKey1);
        RSA_free(privateKey2);
        RSA_free(publicKey2);
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
