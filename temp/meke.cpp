#include <iostream>
#include "project/blockchain/block.hpp"

namespace tin_blockchain
{
    Transaction createGenesisTransaction()
    {
        // Coinbase ScriptSig
        std::string coinbaseMessage = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
        ScriptSig scriptSig(coinbaseMessage, "04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73");

        // Coinbase Vin
        PrevOut prevOut(0, false, 0, -1, "", "", "", {});
        Vin vin(0xffffffff, "", scriptSig.hex, 0, prevOut);

        // Coinbase Vout
        ScriptPubKey scriptPubKey("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f", "4104678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5fac", 1, "pubkey", {"1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa"});
        Vout vout(50, 0, scriptPubKey);

        std::vector<Vin> vinVec = {vin};
        std::vector<Vout> voutVec = {vout};

        Transaction tx("", 1, vinVec.size(), voutVec.size(), 204, 800, 0, "", 0, 0, false, 1231006505, 0, 0, vinVec, voutVec);

        // Compute the transaction hash
        tx.createHash();

        return tx;
    }

    Block createGenesisBlock()
    {
        Transaction genesisTx = createGenesisTransaction();

        BlockHeader header(1, "", genesisTx.hash, 1231006505, 0x1d00ffff, 2083236893);
        std::vector<Transaction> transactions = {genesisTx};

        Block genesisBlock(header, transactions);

        return genesisBlock;
    }
}

int main()
{
    tin_blockchain::Block genesisBlock = tin_blockchain::createGenesisBlock();

    std::cout << "Genesis Block:" << std::endl;
    genesisBlock.printBlock();

    // Optionally, write Genesis Block to JSON file
    genesisBlock.writeToJsonFile("genesis_block.json");

    return 0;
}